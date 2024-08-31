
因作者没及时更新cef 12*版本 我又需要新版本所有自行适配了一下 
使用 qt creator 打开 cmakelist.txt 加载项目 
在项目创建多cef版本 clone debug 添加 -DCEF_SDK_VERSION:UNINITIALIZED=123.0.13+gfc703fb+chromium-123.0.6312.124
再构建即可，初次需要构建两次
