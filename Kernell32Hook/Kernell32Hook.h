// ����������� ���� ���� ifdef - ��� ����������� ����� �������� ��������, ���������� ��������� 
// �������� �� ��������� DLL. ��� ����� ������ DLL �������������� � �������������� ������� KERNELL32HOOK_EXPORTS,
// ��������� � ��������� ������. ������ ������ �� ������ ���� ��������� �� � ����� �������,
// ������������ ������ DLL. ��������� ����� ����� ������ ������, ��� �������� ����� �������� ������ ����, ����� 
// ������� KERNELL32HOOK_API ��� ��������������� �� DLL, ����� ��� ������ DLL ����� �������,
// ������������ ������ ��������, ��� ����������������.
#ifdef KERNELL32HOOK_EXPORTS
#define KERNELL32HOOK_API __declspec(dllexport)
#else
#define KERNELL32HOOK_API __declspec(dllimport)
#endif

// ���� ����� ������������� �� Kernell32Hook.dll
class KERNELL32HOOK_API CKernell32Hook {
public:
	CKernell32Hook(void);
	// TODO. �������� ����� ���� ������.
};

extern KERNELL32HOOK_API int nKernell32Hook;

KERNELL32HOOK_API int fnKernell32Hook(void);
