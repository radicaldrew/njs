
NXT_LIB =	nxt


$(NXT_BUILDDIR)/libnxt.a: \
	$(NXT_BUILDDIR)/nxt_djb_hash.o \
	$(NXT_BUILDDIR)/nxt_utf8.o \
	$(NXT_BUILDDIR)/nxt_array.o \
	$(NXT_BUILDDIR)/nxt_queue.o \
	$(NXT_BUILDDIR)/nxt_rbtree.o \
	$(NXT_BUILDDIR)/nxt_lvlhsh.o \
	$(NXT_BUILDDIR)/nxt_random.o \
	$(NXT_BUILDDIR)/nxt_malloc.o \
	$(NXT_BUILDDIR)/nxt_mem_cache_pool.o \

	@echo "Building archive " $@
	@ar -r -c $@ \
		$(NXT_BUILDDIR)/nxt_djb_hash.o \
		$(NXT_BUILDDIR)/nxt_utf8.o \
		$(NXT_BUILDDIR)/nxt_array.o \
		$(NXT_BUILDDIR)/nxt_rbtree.o \
		$(NXT_BUILDDIR)/nxt_lvlhsh.o \
		$(NXT_BUILDDIR)/nxt_malloc.o \
		$(NXT_BUILDDIR)/nxt_random.o \
		$(NXT_BUILDDIR)/nxt_mem_cache_pool.o \

$(NXT_BUILDDIR)/nxt_murmur_hash.o: \
	$(NXT_LIB)/nxt_types.h \
	$(NXT_LIB)/nxt_clang.h \
	$(NXT_LIB)/nxt_murmur_hash.h \
	$(NXT_LIB)/nxt_murmur_hash.c \


$(NXT_BUILDDIR)/nxt_djb_hash.o: \
	$(NXT_LIB)/nxt_types.h \
	$(NXT_LIB)/nxt_clang.h \
	$(NXT_LIB)/nxt_djb_hash.h \
	$(NXT_LIB)/nxt_djb_hash.c \


$(NXT_BUILDDIR)/nxt_utf8.o: \
	$(NXT_LIB)/nxt_types.h \
	$(NXT_LIB)/nxt_clang.h \
	$(NXT_LIB)/nxt_utf8.h \
	$(NXT_LIB)/nxt_utf8.c \


$(NXT_BUILDDIR)/nxt_array.o: \
	$(NXT_LIB)/nxt_types.h \
	$(NXT_LIB)/nxt_clang.h \
	$(NXT_LIB)/nxt_array.h \
	$(NXT_LIB)/nxt_array.c \


$(NXT_BUILDDIR)/nxt_queue.o: \
	$(NXT_LIB)/nxt_types.h \
	$(NXT_LIB)/nxt_clang.h \
	$(NXT_LIB)/nxt_queue.h \
	$(NXT_LIB)/nxt_queue.c \


$(NXT_BUILDDIR)/nxt_rbtree.o: \
	$(NXT_LIB)/nxt_types.h \
	$(NXT_LIB)/nxt_clang.h \
	$(NXT_LIB)/nxt_rbtree.h \
	$(NXT_LIB)/nxt_rbtree.c \


$(NXT_BUILDDIR)/nxt_lvlhsh.o: \
	$(NXT_LIB)/nxt_types.h \
	$(NXT_LIB)/nxt_clang.h \
	$(NXT_LIB)/nxt_lvlhsh.h \
	$(NXT_LIB)/nxt_lvlhsh.c \


$(NXT_BUILDDIR)/nxt_malloc.o: \
	$(NXT_LIB)/nxt_auto_config.h \
	$(NXT_LIB)/nxt_types.h \
	$(NXT_LIB)/nxt_clang.h \
	$(NXT_LIB)/nxt_malloc.h \
	$(NXT_LIB)/nxt_malloc.c \


$(NXT_BUILDDIR)/nxt_random.o: \
	$(NXT_LIB)/nxt_types.h \
	$(NXT_LIB)/nxt_clang.h \
	$(NXT_LIB)/nxt_random.h \
	$(NXT_LIB)/nxt_random.c \


$(NXT_BUILDDIR)/nxt_mem_cache_pool.o: \
	$(NXT_LIB)/nxt_types.h \
	$(NXT_LIB)/nxt_clang.h \
	$(NXT_LIB)/nxt_alignment.h \
	$(NXT_LIB)/nxt_queue.h \
	$(NXT_LIB)/nxt_rbtree.h \
	$(NXT_LIB)/nxt_mem_cache_pool.h \
	$(NXT_LIB)/nxt_mem_cache_pool.c \



$(NXT_BUILDDIR)/%.o: $(NXT_LIB)/%.c
	@echo "Compiling $<"
	@$(NXT_CC) -c -o $@ $(NXT_CFLAGS) -I$(NXT_LIB) $<


include $(NXT_LIB)/test/Makefile
