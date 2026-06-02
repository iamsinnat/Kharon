#include <Kharon.h>
 
namespace Shellcode {
    BOOL Load() {
        DbgPrint( "Shellcode from section — Data: 0x%p Size: %llu\n", Data, Size );
        return Data != nullptr && Size > 0;
    }
}
 