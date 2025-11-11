# arm-core

General header and template files provided by arm's CMSIS package. The header files are located at **Core/Include**, and template files are located at **Core/Template**

Not all header files are needed, we are only using **core_cm0plus.h**, which includes, either directly or indirectly, **cmsis_compiler.h**, **cmsis_version.h**, **cmsis_gcc.h**, and **m-profile/cmsis_gcc_m.h**

Any template file used from this folder is saved in either src/ or include/ folders after they have been modified and renamed.

# samd21-core

General header files and start-up files. The header files for this project are located at samd21a/include.

We are mainly using **samd21e17a.h**, which includes all the files in the **component/** and **instance/** folders, as well as **pio/samd21e17a.h**

Since I had to modify the linker and the system code, I copied them to the linker/ and src/ folder, and kept the original version unmodified here.