# poor benchmark

## mypool and malloc

- alloc by g_poolsize_map (PAGE_SIZE = _32KB)
<img src='plot_msvc_2_32KB_2.png'>
<img src='plot_linux_2_32KB_2.png'>


- alloc by 2**N (PAGE_SIZE = _32KB)
<img src='plot_msvc_2_32KB_2_2.png'>
<img src='plot_linux_2_32KB.png'>