#ifndef DX_ERROR_CHECK_H
#define DX_ERROR_CHECK_H



#define D3D_SKIP( condition ) if ( _hResult == D3D_OK && condition ) { goto d3d_error_check_skip;}

#define D3D_CHECK_BEGIN( test_statement )   \
    {                                       \
        HRESULT _hResult = Engine()->HasFailed() ? D3D_OK : test_statement;    \
        if ( FAILED(_hResult) )               \
        {


#if 0
            wchar_t buffer[255]; \
            wchar_t *pBuffer = &buffer[0]; \
            _snwprintf_s(pBuffer, 255, 255, L"D3D Error Code: %l\n", _hResult); \
            Engine()->Debug(buffer);
#endif

//#define D3D_CHECK_END() d3d_error_check_skip:{} }
#define D3D_CHECK_END() }}

#define ERROR_BASE( code, message ) Engine()->Error( message );
#define ABORT_BASE( code, message ) Engine()->FatalError( message ); Engine()->SetFailed(); return 0;

#define _D3D_CHECK_BASE(code, message, handler) \
    if ( _hResult == code ) \
    {   \
        handler( code, message ); \
    }  

#define D3D_CHECK(code, message) _D3D_CHECK_BASE( code, message, ERROR_BASE );
#define D3D_ABORT(code, message) _D3D_CHECK_BASE( code, message, ABORT_BASE );



#endif // DX_ERROR_CHECK_H