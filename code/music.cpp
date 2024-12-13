#include "music.h"
using namespace std;
int main(){
    // BGM bgm("../songs/3.txt");
    // bgm.play();
    // while (1);
    // bgm.stop();
    std::string str = "0_ [2^5]_ [2^5]_ 1^__ [2^5]_. 1^_ [2^5]_ 3^_ | [6^3^6#]_. 5_. ";
    MusicPlayer musicPlayer;
    std::vector <NoteList>listVector = musicPlayer.parseLineToListVector(str);
    for(const auto& element : listVector){
        cout << "helloworld\n";
        element.display();
    }
    return 0;
}
