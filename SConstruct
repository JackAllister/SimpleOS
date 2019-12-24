import os


# Create a new environment for building the bootloader.
env = Environment(ENV = os.environ)

# Configure all the tools.
env['CC'] = 'x86_64-w64-mingw32-gcc'
env['LINK'] = 'ld'

# Compiler include paths.
env.Append(CPPPATH = [
    '/usr/include/efi/', 
    '/usr/include/efi/x86_64/'
])

# Compiler flags.
env.Append(CFLAGS = [
    '-fno-stack-protector',
    '-fpic',
    '-fshort-wchar',
    '-mno-red-zone'
])

# Configure the linker.
env.Append(LINKFLAGS = [
    '/usr/lib/crt0-efi-x86_64.o',
    '-nostdlib',
    '-znocombreloc',
    '-shared',
    '-Bsymbolic',
    '-T/usr/lib/elf_x86_64_efi.lds'
])

# Configure the libraries.
env.Append(LIBPATH = [
    '/usr/lib/'
])

env.Append(LIBS = [
    'libgnuefi.a',
    'libefi.a'
])

# Set the format for the linking.
env['LINKCOM'] = '$LINK $SOURCES $LINKFLAGS $_LIBDIRFLAGS $_LIBFLAGS -o $TARGET'

# Create an object file for all the source files.
env.Program('output/boot.so', Glob('bootloader/src/*.c'))


# Create a new environment for building the kernel.