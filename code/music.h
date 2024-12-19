#pragma once  // 防止头文件被多次包含
#include <Windows.h>  // Windows API 头文件
#include <thread>     // 线程支持
#include <conio.h>    // 控制台输入输出支持
#include <mutex>      // 线程互斥锁
#include <regex>      // 正则表达式支持
#include <string.h>   // 字符串操作
#include <fstream>    // 文件读写支持
#include <iostream>   // 输入输出流支持
#include <assert.h>   // 断言支持
#include "LinkedList.h"
#include "NoteList.h"
#include "MusicProcess.h"
#define DEBUG 0       // 调试标志，0 表示关闭调试信息

//音符列表类（乐谱类）
class MusicManager{
public:
	std::string musicPath;
	LinkedList currentMusic; //存储音符的链表
	MusicSerializer musicSerializer;
	MusicDeserializer musicDeserializer;
	~MusicManager(){}

	//添加文本行至链表
	void add(std::string s){
		currentMusic.append(s);
	}

	//清除乐谱
	void clear(){
		currentMusic.clear();
	}
	

	//读取乐谱，读取相应文件名的文件，如无参数，默认为空
	void load(std::string path=""){
		//清除当前乐谱
		clear();
		//打开文件
		std::ifstream in(path);
		std::string noteLine;
		//读取空格之间的字符串作为音符，并加入乐谱
		while (getline(in,noteLine)) add(noteLine);
		//关闭io流
		in.close();
	}
	//调用此函数前必须先将currentMusic调用clear()函数，以防万一
	void saveLine(std::vector<NoteList> noteTable){
		std::string musicStr =  musicSerializer.serialize(noteTable);
		currentMusic.append(musicStr);
	}
	void saveMusic(){
		for (int i = 0; i < currentMusic.size(); i++)
		{
			std::string musicStr = currentMusic.findAt(i);

		}
		
	}
	void create(std::string music){
		
	}
	void remove(std::string music){

	}
	//查找并加载歌曲
	void query(std::string music){
		
	}
	//构造函数
	MusicManager(std::string path=""){
		currentMusic.clear();
		if (path!="") load(path);
	}
};
//判断是否为纯数字的节奏行
bool isNumeric(std::string const &str){
	return regex_match(str,std::regex("[0-9]+"));
}

class MusicPlayer{
private:
	//midi输出句柄
	HMIDIOUT handle;

public:

	//音乐播放状态（是否结束）
	bool ENDMUSIC=0;
	//停止播放命令
	int STOP;
	//构造函数，当创建MusicPlayer对象时，自动打开midi输出设备
	MusicPlayer(){
		midiOutOpen(&handle,0,0,0,CALLBACK_NULL);
	}
	//对象被销毁时，关闭midi设备
	~MusicPlayer(){
		midiOutClose(handle);
	}


	//互斥锁，保护多线程共享资源
	std::mutex mu;

	int tick1,tick2;
	MusicDeserializer musicDeserializer;
	void play(std::string s1,std::string s2="",double duration = 500){
		STOP=0;tick1=0;tick2=0;
		//设置曲子线程，分别对应两段旋律
		std::thread tune1(&MusicPlayer::playSingleLine,this,musicDeserializer.deserialize(s1,duration),1);tune1.detach();
		std::thread tune2(&MusicPlayer::playSingleLine,this,musicDeserializer.deserialize(s2,duration),0);tune2.detach();
		while (STOP<2);
		if (DEBUG){
			if (tick1==tick2) puts("Succ");
			else printf("Warn: %d!=%d\n",tick1,tick2);			
		}
	}
	
	void classifyList(MusicManager &m){
		ENDMUSIC=0;
		double duration = 500;
		for (int i=0;i<m.currentMusic.size() && !ENDMUSIC;++i){
			
			//解析节奏行，跳过空行
			while (i<m.currentMusic.size() && (m.currentMusic.findAt(i) == "" || isNumeric(m.currentMusic.findAt(i)))){
				if (isNumeric(m.currentMusic.findAt(i))){
					duration = stod(m.currentMusic.findAt(i));
				}
				i++;
			}
			//结束循环，防止越界
			if (i>=m.currentMusic.size()) break;
			//获取当前行存储至至s1，如果下一行也不为空，解析并存储至s2；s1和s2分别为两段旋律
			std::string s1=m.currentMusic.findAt(i),s2="";
			if (i<m.currentMusic.size()-1 && m.currentMusic.findAt(i+1)!="") s2=m.currentMusic.findAt(i+1),i++;
			//播放s1和s2
			play(s1,s2,duration);
			// duration = 500;
		}
	}

	void playSingleLine(std::vector<NoteList> noteTable, bool isMain){
		//获取播放开始的时间戳
		long start = clock();
		//定义记录时间戳，跟随进度
		double tick = 0;
		//遍历Note中的所有和弦的音符，发送至midi统一播放
		for(NoteList noteList : noteTable){
			std::vector<int> noteBuffer = noteList.transformNoteBuffer();
			for(int noteInfo : noteBuffer){
				if(noteInfo != 0){
					midiOutShortMsg(handle, noteInfo);
				}
			}
			noteBuffer.clear();
			// std::string tune = isMain ? "tune1" : "tune2";
			// std::cout << "duration " + tune + "\t" << tick + noteList.getDuration() + '\n'<<std::endl;
			//设置音符播放持续时间
			while ((clock() - start) * 1.0 < tick + noteList.getDuration());
			//更新进度条
			tick += noteList.getDuration(); 
		}
		//如果本行是主旋律，则本行的时间戳为主时间戳
		if (isMain) tick1=tick;else tick2=tick;
		//线程锁，等待下一个线程就绪
		mu.lock();
		//两个线程就绪（stop=2），开始解锁
		STOP++;
		mu.unlock();
	}
};
class BGM{
public:
	MusicPlayer player;
	MusicManager nowList;
	BGM(std::string name){
		nowList.load(name);
	}
	~BGM(){
		player.ENDMUSIC=1;
	}
	void setMusic(std::string name){
		nowList.load(name);
	}
	void play_thread(){
		while (1){
			player.classifyList(nowList);
			if (player.ENDMUSIC) break;	
		}
	}
	void play(){
		player.ENDMUSIC=0;
		std::thread bgm(&BGM::play_thread,this);bgm.detach();
	}
	void stop(){
		player.ENDMUSIC=1;
	}
};

