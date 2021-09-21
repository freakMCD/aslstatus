# creating component

create file with `.c` extension in [components/](../components/)

## basic example

`components/simple_module.c`:
```c
#include "../lib/util.h"  /* you can find some useful functions in `util.c` */

void
function_name(char *out, const char __unused *_a,
		uint32_t __unused _i, void __unused *_p)
{
	bprintf(out, "%s", "Hello, World!");
}
```

`arg`, `interval` and `static_ptr` is optional argument
and if it is unused add `__unused` attribute

then put:
```c
void function_name FUNC_ARGS;
#define function_name {function_name, "thread name", 0}
```

at the end of [aslstatus.h](aslstatus.h)


#### complex example

for complex example see [brightness](../components/brightness.c)
and then [entropy](../components/entropy.c) component
(this components are "simple" and have detailed comments)


`aslstatus.h`:
```c
/* ... */

void my_module FUNC_ARGS;
#define my_module {my_module, "My Module", static_data_size}
```

in `#define`:
1. component function (should be same as define name)
2. thread name (can be same as component function name)
3. size of static data (described in [brightness components sources](../components/brightness.c))
