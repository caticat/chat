#pragma once

#ifdef DLL_EXPORT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

// 初始化传输数据类
#define INIT_CP(type) do {memset(&type,0,sizeof(type));} while (0)
