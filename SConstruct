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
    'bootloader',
    'libraries/bootstrap',
    'libraries/draw',
    'libraries/osTypes'
])

# Compiler flags.
env.Append(CFLAGS = [
    '-fno-stack-protector',
    '-fpic',
    '-fshort-wchar',
    '-mno-red-zone',
    '-ggdb'
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

# Gather a list of all the source files.
sourceList = []

# Append the bootloader files
sourceList.append(Glob('bootloader/*.c'))

# Append the library files
sourceList.append(Glob('libraries/*/*.c'))

# Create an object file for all the source files.
generatedProgram = env.Program('output/boot.so', sourceList)

# UEFI file generator.

def uefi_generator(source, target, env, for_signature):
    
    sectionsFlags = ''

    # Define which sections to copy.
    sectionsToCopy = ['.text', '.sdata', '.data', '.dynamic', '.dynsym', '.rel', '.rela', '.reloc']

    for section in sectionsToCopy:
        sectionsFlags += '-j %s '%(section)

    return '$OBJCOPY %s --target=efi-app-x86_64 %s %s'%(sectionsFlags, source[0], target[0])

def debug_uefi_generator(source, target, env, for_signature):

    sectionsFlags = ''

    # Define which sections to copy.
    sectionsToCopy = ['.text', '.sdata', '.data', '.dynamic', '.dynsym', '.rel', '.rela', '.reloc',
                      '.debug_info', '.debug_abbrev', '.debug_loc', '.debug_aranges', '.debug_line', '.debug_macinfo', '.debug_str']

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

env.Append(BUILDERS = {
    'DebugUefiBuild': Builder(
        generator=debug_uefi_generator,
        suffix='.efi.debug',
        src_suffix='.so'
    )
})


# Use the UEFI file generator.
env.UefiBuild(generatedProgram)

env.DebugUefiBuild(generatedProgram)

# Create a new environment for building the kernel.