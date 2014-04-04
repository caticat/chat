#pragma once

#ifdef DLL_EXPORT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

// 初始化传输数据类
#define INIT_CP(type) do {memset(&type,0,sizeof(type));} while (0)

// 注册函数处理用
#define STITLE_HUB(title, fun, user, msg) do {if (title == msg.GetTitle()) fun(user,msg);} while (0)
#define CTITLE_HUB(title, fun, msg) do {if (title == msg.GetTitle()) fun(msg);} while (0)
