# Cronología

* 1974 - Paper de Unix
* 1978 - Libro K&R C
* 1983 - Unix System V
* 1989 - GPLv1 (Libertades básicas, licencias virales, etc)
* 1991 - Linux
* 1992 - Linux 0.12 GPL
* 2001 - Linux 2.4 (“Linux is a Cancer” - Microsoft CEO)
* 2005 - Git - The stupid content tracker
* 2008 - Github
* 2018 - Microsoft compra Github
* 2019 - Linux 5.0

# ¿Qué es un kernel?

*Diagrama de capas y de cómo las cosas es comunican entre sí.*

# Ejercicios navegando en el sourcetree del kernel

Se puede usar `git-grep` para buscar en el código fuente:

1. Buscar la definición de `struct device`:

```
iglosiggio@mega-tmov ~/e/c/linux> git grep 'struct device {'
include/linux/device.h:struct device {
tools/virtio/linux/virtio.h:struct device {
```

2. Buscar al autor de los releases `v4.17`, `v4.18`, `v4.19`, `v4.20`:

```
iglosiggio@mega-tmob ~/e/c/linux> git show v4.{17,18,19,20} | grep -1 Tagger
tag v4.17
Tagger: Linus Torvalds <torvalds@linux-foundation.org>
Date:   Sun Jun 3 14:15:30 2018 -0700
--
tag v4.18
Tagger: Linus Torvalds <torvalds@linux-foundation.org>
Date:   Sun Aug 12 13:41:12 2018 -0700
--
tag v4.19
Tagger: Greg Kroah-Hartman <gregkh@linuxfoundation.org>
Date:   Mon Oct 22 07:47:45 2018 +0100
--
tag v4.20
Tagger: Linus Torvalds <torvalds@linux-foundation.org>
Date:   Sun Dec 23 15:56:06 2018 -0800
```

3. Buscar la cantidad de commits hechos en las arquitecturas `arm`, `arm64`,
   `x86` y `mips` para la versión `v4.20`:

```
iglosiggio@mega-tmob ~/e/c/l/arch> git log --oneline v4.19..v4.20 arch/arm | wc -l
739
iglosiggio@mega-tmob ~/e/c/l/arch> git log --oneline v4.19..v4.20 arch/arm64 | wc -l
551
iglosiggio@mega-tmob ~/e/c/l/arch> git log --oneline v4.19..v4.20 arch/x86 | wc -l
530
iglosiggio@mega-tmob ~/e/c/l/arch> git log --oneline v4.19..v4.20 arch/mips | wc -l
127
```

4. Buscar el primer commit en tu clon del repositorio de Linux:

```
iglosiggio@mega-tmob ~/e/c/l/arch> git rev-list --all | tail -1
1da177e4c3f41524e886b7f1b8a0c1fc7321cac2
iglosiggio@mega-tmob ~/e/c/l/arch> git show 1da177e4c3f41524e886b7f1b8a0c1fc7321cac2
commit 1da177e4c3f41524e886b7f1b8a0c1fc7321cac2 (tag: v2.6.12-rc2)
Author: Linus Torvalds <torvalds@ppc970.osdl.org>
Date:   Sat Apr 16 15:20:36 2005 -0700

    Linux-2.6.12-rc2

    Initial git repository build. I'm not bothering with the full history,
    even though we have it. We can create a separate "historical" git
    archive of that later if we want to, and in the meantime it's about
    3.2GB when imported into git - space that would just make the early
    git days unnecessarily complicated, when we don't have a lot of good
    infrastructure for it.

    Let it rip!

```

5. ¿Cuánto de `init/main.c` sobrevivió desde que se agregó al repositorio?

```
iglosiggio@mega-tmob ~/e/c/linux> git show 1da177e4c3f41524e886b7f1b8a0c1fc7321cac2:init/main.c | wc -l
713
iglosiggio@mega-tmob ~/e/c/linux> git diff 1da177e4c3f41524e886b7f1b8a0c1fc7321cac2:init/main.c init/main.c | grep '^-' | wc -l
361
iglosiggio@mega-tmob ~/e/c/linux> bc
bc 1.07.1
Copyright 1991-1994, 1997, 1998, 2000, 2004, 2006, 2008, 2012-2017 Free Software Foundation, Inc.
This is free software with ABSOLUTELY NO WARRANTY.
For details type `warranty'.
scale=5
361 / 713
.50631
```

6. Los commits no deben romper la “booteabilidad” del kernel, estando parado en
   un branch del mismo ¿Cómo se puede verificar si esta propiedad se cumple? 

Hint: git rebase --exec

*Discusión sobre la rebasabilidad del kernel, cómo configurar y contribuir
cambios.*
*Discusión sobre los flags de configuración y los mecanismos de carga/descarga
de módulos.*

7. Buscar la opción CONFIG\_MULTIUSER ¿Cuáles son sus dependencias? ¿Cuándo se
   introdujo? ¿Cómo funciona (en sentido general)?

Buscar la opción:
```
iglosiggio@mega-tmob ~/e/c/linux> grep -A13 'config MULTIUSER' init/Kconfig
config MULTIUSER
        bool "Multiple users, groups and capabilities support" if EXPERT
        default y
        help
          This option enables support for non-root users, groups and
          capabilities.

          If you say N here, all processes will run with UID 0, GID 0, and all
          possible capabilities.  Saying N here also compiles out support for
          system calls related to UIDs, GIDs, and capabilities, such as setuid,
          setgid, and capset.

          If unsure, say Y here.
```

Buscar el commit:
```
git blame init/Kconfig | grep 'config MULTIUSER'
2813893f8b197 (Iulia Manda 2015-04-15 16:16:41 -0700 1206) config MULTIUSER
```

Diffstat:
```
iglosiggio@mega-tmob ~/e/c/linux> git show 2813893f8b197 --stat --oneline
2813893f8b19 kernel: conditionally support non-root users, groups and capabilities
 arch/s390/Kconfig                     |  1 +
 drivers/staging/lustre/lustre/Kconfig |  1 +
 fs/nfs/Kconfig                        |  2 +-
 fs/nfsd/Kconfig                       |  1 +
 include/linux/capability.h            | 29 +++++++++++++++++++++++++++++
 include/linux/cred.h                  | 23 +++++++++++++++++++----
 include/linux/uidgid.h                | 12 ++++++++++++
 init/Kconfig                          | 19 ++++++++++++++++++-
 kernel/Makefile                       |  4 +++-
 kernel/capability.c                   | 35 +++++++++++++++++++----------------
 kernel/cred.c                         |  3 +++
 kernel/groups.c                       |  3 ---
 kernel/sys.c                          |  2 ++
 kernel/sys_ni.c                       | 14 ++++++++++++++
 net/sunrpc/Kconfig                    |  2 ++
 security/Kconfig                      |  1 +
 16 files changed, 126 insertions(+), 26 deletions(-)
```

Recomendación de Ezequiel: “#ifdef considered harmful”.

8. Hacer una opción `CONFIG_REBOOT` que deshabilte la posibilidad de hacer
   `reboot`.

* En `kernel/reboot.c` está el código de la syscall. ¿Es suficiente
  deshabilitarla?
* La syscall tiene código de otras opciones. ¿Deberían depender de
  `CONFIG_REBOOT`?
* En `kernel/sys_ni.c` se especifican las llamadas del sistema que son
  opcionales, se debe agregar `reboot()`

Después de estos cambios `scripts/bloat-o-meter` arroja la siguiente
comparación:

```
add/remove: 0/5 grow/shrink: 0/0 up/down: 0/-451 (-451)
Function                                     old     new   delta
_eil_addr___x64_sys_reboot                    16       -     -16
_eil_addr___ia32_sys_reboot                   16       -     -16
__ia32_sys_reboot                             20       -     -20
__x64_sys_reboot                              21       -     -21
__do_sys_reboot                              378       -    -378
Total: Before=13351720, After=13351269, chg -0.00%
```

¿Habría que cambiar más código? ¿Vale la pena *ifdefear* más cosas?

# Discusiones sobre el kernel

## Estructura de carpetas del kernel

* __arch/__: Código dependiente de la arquitectura.
* __block/__: ???
* __drivers/__: Drivers de dispositivos.
* __firmware/__: ??
* __fs/__: Soporte de filesystems.
* __include/__: Headers compartidos con definiciones básicas.
* __init/__: ???
* __ipc/__: ???
* __kernel/__: Código core del kernel (scheduler, syscalls, etc).
* __lib/__: ???
* __mm/__: Código para manejo de memoria.
* __net/__: Código que implementa los protocolos de red.

## Mainline kernel: upstream y downstream
__RELLENAR__

## Tipos de dispositivos

Hay una capa de frameworks de drivers que van uno dentro del otro. Por ejemplo,
si tenemos una placa de red sus drivers están sobre la infraestructura de
ethernet/wifi/etc. Cada capa de abstracción es común entre otros drivers del
mismo tipo.

### block vs char devices
Los `char devices` reciben las syscalls normales pero no están buffereadas.

# Ejercicios haciendo módulos
_Nota:_ Auto-registración de char devices: **enviado por email**

## Hacer el driver de ejemplo `hello_world` _in-tree_

Se puede encontrar el commit correspondiente en la rama
`linux/iglosiggio/hello-world` de este repo. El ejercicio trata sobre reconocer
los patrones utilizados para la construcción de módulos e intentar replicarlos
integrando el módulo de juguete.

## Identificar los bugs del driver `yoda_buggy` e intentar fixearlos.

**TODO:** tomar notas sobre tips y ayudas para completar el ejercicio.


## ¿Qué pasa cuando se llama a `write(2)` sobre un nodo de `yoda_buggy`?  ¿Dónde está ese código?

**Nota:** Soy un tarado y escribí que pasa cuando se hace `read(2)`, si bien es
similar lo que pasa (gran parte del código es un calco letra a letra) debería
actualizar los cachos de código y el texto de acá abajo.

Busquemos la syscall de read:

```
iglosiggio@mega-tmob ~/e/c/linux> git grep SYSCALL_DEFINE3\(read
fs/read_write.c:SYSCALL_DEFINE3(read, unsigned int, fd, char __user *, buf, size_t, count)
fs/read_write.c:SYSCALL_DEFINE3(readv, unsigned long, fd, const struct iovec __user *, vec,
fs/read_write.c:COMPAT_SYSCALL_DEFINE3(readv, compat_ulong_t, fd,
fs/stat.c:SYSCALL_DEFINE3(readlink, const char __user *, path, char __user *, buf,
mm/readahead.c:SYSCALL_DEFINE3(readahead, int, fd, loff_t, offset, size_t, count)
```

¿Qué corno hace read?

```
ssize_t ksys_read(unsigned int fd, char __user *buf, size_t count)
{
        struct fd f = fdget_pos(fd);
        ssize_t ret = -EBADF;

        if (f.file) {
                loff_t pos = file_pos_read(f.file);
                ret = vfs_read(f.file, buf, count, &pos);
                if (ret >= 0)
                        file_pos_write(f.file, pos);
                fdput_pos(f);
        }
        return ret;
}

SYSCALL_DEFINE3(read, unsigned int, fd, char __user *, buf, size_t, count)
{
        return ksys_read(fd, buf, count);
}
```

`fdget_pos` consigue el fichero pedio, si no existe se devuelve `EBADF` (Bad
file descriptor), el resto del código se encarga de actualizar la posición
y el fichero de acuerdo a los cambios de genere `vfs_read`.

Entonces ¿Qué va hace `vfs_read`?

1. Chequea que el archivo se haya abierto de forma que se pueda leer
   (`FMODE_READ`), en caso contrario devuelve `EBADF` (Bad file descriptor).
2. Chequea que el archivo tenga un método para leerlo (podemos tener archivos
   sólamente escribibles), si esto no ocurre devuelve `EINVAL` (Invalid
   Argument).
3. Chequea que el puntero pasado como buffer sea válido, caso contrario
   devuelve `EFAULT` (Bad address).
4. Chequea que el file descriptor tenga los permisos correspondientes y si todo
   sale bien corre `__vfs_read` y las notificaciones correspondientes
   (fsnotify, io del proceso, etc).

Bueno, una capa menos, ahora por `__vfs_read`, por suerte se explica (casi)
sólo:

```
if (file->f_op->read)
        return file->f_op->read(file, buf, count, pos);
else if (file->f_op->read_iter)
        return new_sync_read(file, buf, count, pos);
else
        return -EINVAL;
```

Si las operaciones registradas de ese archivo tienen a `read` entonces se
utiliza para completar la operación, si tienen a `read_iter` ese es elegido y
si no ocurre ninguna de las dos entonces ese archivo no es legible y se
devuelve `EINVAL` (Invalid argument).

En nuestro driver de jugete `yoda_buggy` la implementación exponía un read, y
así es cómo se llevaba a cabo la magia correspondiente.

## Descargar módulos del kernel

**TODO:** Queda para la próxima clase del curso.
