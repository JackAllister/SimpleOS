import os


# Create a new environment for building the bootloader.
env = Environment(ENV = os.environ)

# Configure all the tools.
env['CC'] = 'gcc'
env['LINK'] = 'ld'
env['OBJCOPY'] = 'objcopy'

# Compiler include paths.
env.Append(CPPPATH = [
    '/usr/include/efi/', 
    '/usr/include/efi/x86_64/',
    'bootloader/header'
])

# Compiler flags.
env.Append(CFLAGS = [
    '-fno-stack-protector',
    '-fpic',
    '-fshort-wchar',
    '-mno-red-zone'
])

# Compiler defines.
env.Append(CPPDEFINES = [
    'EFI_FUNCTION_WRAPPER'
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
generatedProgram = env.Program('output/boot.so', Glob('bootloader/src/*.c'))

# UEFI file generator.

def uefi_generator(source, target, env, for_signature):
    
    sectionsFlags = ''

    # Define which sections to copy.
    sectionsToCopy = ['.text', '.sdata', '.data', '.dynamic', '.dynsym', '.rel', '.rela', '.reloc']

    for section in sectionsToCopy:
        sectionsFlags += '-j %s '%(section)

    return '$OBJCOPY %s --target=efi-app-x86_64 %s %s'%(sectionsFlags, source[0], target[0])

env.Append(BUILDERS = {
    'UefiBuild': Builder(
        generator=uefi_generator,
        suffix='.efi',
        src_suffix='.so'
    )
})

# Use the UEFI file generator.
env.UefiBuild(generatedProgram)

# Create a new environment for building the kernel.