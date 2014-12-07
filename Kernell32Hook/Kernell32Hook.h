// Приведенный ниже блок ifdef - это стандартный метод создания макросов, упрощающий процедуру 
// экспорта из библиотек DLL. Все файлы данной DLL скомпилированы с использованием символа KERNELL32HOOK_EXPORTS,
// заданного в командной строке. Данный символ не должен быть определен ни в одном проекте,
// использующем данную DLL. Благодаря этому любой другой проект, чьи исходные файлы включают данный файл, видит 
// функции KERNELL32HOOK_API как импортированные из DLL, тогда как данная DLL видит символы,
// определяемые данным макросом, как экспортированные.
#ifdef KERNELL32HOOK_EXPORTS
#define KERNELL32HOOK_API __declspec(dllexport)
#else
#define KERNELL32HOOK_API __declspec(dllimport)
#endif

// Этот класс экспортирован из Kernell32Hook.dll
class KERNELL32HOOK_API CKernell32Hook {
public:
	CKernell32Hook(void);
	// TODO. Добавьте здесь свои методы.
};

extern KERNELL32HOOK_API int nKernell32Hook;

KERNELL32HOOK_API int fnKernell32Hook(void);
