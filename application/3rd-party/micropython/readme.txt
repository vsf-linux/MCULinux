Pre-defines:
MICROPY_PY_TERMIOS=1
MICROPY_PY_SOCKET=1
MICROPY_PY_THREAD=1
MICROPY_PY_THREAD_GIL=0
MICROPY_PY_SSL=1
MICROPY_SSL_MBEDTLS=1

Include-Directories, ${variant} in [minimal, standard]:
$(ProjectDir)..\..\application\3rd-party\micropython\port
$(ProjectDir)..\..\application\3rd-party\micropython\port\${variant}
$(ProjectDir)..\..\application\3rd-party\micropython\raw
$(ProjectDir)..\..\application\3rd-party\micropython\raw\ports\unix
$(ProjectDir)..\..\application\3rd-party\micropython\raw\ports\unix\variants\${variant}

Source-Codes:
micropython\extmod
micropython\shared
micropython\ports\unix
micropython\py
mbedtls
