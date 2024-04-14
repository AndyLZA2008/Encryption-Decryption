//
// Created by AndyLZA on 2022/10/5.
//

#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <direct.h>
#include <windows.h>

using namespace std;

const char YES = 'Y';
const char NO = 'N';

bool remixHistory(char op);

void noWay();

void _wait();

int main() {
    cout << "输入字符 Y表示肯定、输入字符 N 表示否定，否则自动退出。\n"
        "=================================================\n"
        "看起来您遇到了一些问题，我们也许可以提供一些帮助。\n"
        "在此之前，请把此文件与主程序放到同一个文件夹中。\n"
        "是否继续？\n";
    char op;
    cin >> op;
    _wait();
    if (remixHistory(op)) {
        cout << "我们已经尝试解决了一些问题，这是否有效？\n";
        cin >> op;
        _wait();
        if (op == NO) {
            cout << "抱歉，请问您的问题属于哪一种？\n"
                "1、图像错误；2、历史记录错误(即“字体”“加密率”“秘钥”显示错误)；3、复制后中文乱码问题；4这里没有我的问题。\n"
                "请输入数字1-4\n";
            cin >> op;
            _wait();
            if (op == '1') {
                cout << "请重新下载此文程序，这是否有效？\n";
                cin >> op;
                _wait();
                if (op == NO) {
                    if (op == NO) {
                        noWay();
                        return 0;
                    }
                }
            }
            else if (op == '2') {
                cout << "我们已经尝试解决了一些问题，这是否有效？\n";
                cin >> op;
                _wait();
                if (op == NO) {
                    noWay();
                    return 0;
                }
            }
            else if (op == '3') {
                cout << "请在中文输入法下打开程序重试，这是否有效？\n";
                cin >> op;
                _wait();
                if (op == NO) {
                    noWay();
                    return 0;
                }
            }
            else if (op == '4') {
                noWay();
                return 0;
            }
        }
    }
    cout << "很高兴帮您解决了问题，感谢您的使用，再见。";
    getchar();
    getchar();
    return 0;
}

bool remixHistory(char op) {
    if (op == YES) {
        string folderPath = getenv("USERPROFILE");
        for (int i = 0; i < folderPath.size(); ++i) {
            if ((int)folderPath[i] == 92) {
                folderPath[i] = '/';
            }
        }
        folderPath = folderPath + "/" + ".ed";
        if (0 != _mkdir(folderPath.c_str())) {
            _mkdir(folderPath.c_str());
        }
        string file_name = folderPath + "/Personalized_History";
        ofstream file_writer(file_name, ios_base::out);
        file_writer.close();
        return true;
    }
    else {
        return false;
    }
}

void noWay() {
    cout << "抱歉，请尝试重装文件，或联系发布者 andylza@outlook.com 以获取更多帮助。\n再见。";
    getchar();
    getchar();
    return;
}

void _wait() {
    const int sleepTime = 250;
    cout << "请稍后";
    Sleep(sleepTime);
    cout << ".";
    Sleep(sleepTime);
    cout << ".";
    Sleep(sleepTime);
    cout << ".\n";
    Sleep(sleepTime);
    return;
}