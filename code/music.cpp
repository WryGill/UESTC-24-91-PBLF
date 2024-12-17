#include "music.h"
using namespace std;
int main(){
    BGM bgm("../songs/7.txt");
    bgm.play();
    while (1);
    bgm.stop();
    // MusicPlayer musicPlayer;
    // std::string s1 = "6^_. 6^_. 5^_ 6^- | 6 6_. 5__ 6_ 1^_ 1^_ 2^_ | 6 6_. 5__ 6_ 5_ 3_ 5_ | 6 6_. 5__ 6_ 1^_ 2^_ 3^_ |";
    // std::string s2 = "6,,_ 3,_ 6,,_ 3,_ 6,,_ 3,_ 6,,_ 3,_ | 6,,_ 3,_ 6,,_ 3,_ 6,,_ 3,_ 6,,_ 3,_ | 4,,_ [4,1,]_ 4,,_ [4,1,]_ 5,,_ 2,_ 5,,_ 2,_ | 6,,_ 3,_ 6,,_ 3,_ 6,,_ 3,_ 6,,_ 3,_ | ";
    // musicPlayer.play(s1,s2);
    return 0;
}
