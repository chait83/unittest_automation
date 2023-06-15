# ------------------------------------------------------------------------------
# function check_duplicate
#    $(1)  <list>
#    $(2)  <pretext>
#
# example:
#    $(call check_duplicate,src/file1.c,dst/file2.c)
# ------------------------------------------------------------------------------
define check_duplicate
	$(foreach file,$(1), \
		$(if $(word 2,$(filter $(file),$(1))), \
			$(error $(2): $(file)) \
		)\
	)
endef

# ------------------------------------------------------------------------------
# function compile_as
#    $(1)  <o_target>
#    $(2)  <S_source>
#    $(3)  <flags>
#
# example:
#    $(call compile_as,build/file.o,file.S,--pedantic)
# ------------------------------------------------------------------------------
define compile_as
	$(call print_command,AS,$(2),-o,$(1),-c)
	$(Q)$(call ensure_dir,$(1))
	$(Q)$(AS) -c -o $(1:.o=.d) $(2) $(CPPFLAGS) $(ASFLAGS) $(3) \
	          -Xmake-dependency=2                         \
	          -Xmake-dependency-target="$(1)"
	$(Q)$(AS) -c -o $(1) $(2) $(CPPFLAGS) $(ASFLAGS) $(3)
endef

# ------------------------------------------------------------------------------
# function compile_c
#    $(1)  <o_target>
#    $(2)  <c_source>
#    $(3)  <flags>
#
# example:
#    $(call compile_c,build/file.o,file.c,--pedantic)
# ------------------------------------------------------------------------------
define compile_c
	$(call print_command,CC,$(2),-o,$(1),-c)
	$(Q)$(call ensure_dir, $(1))
	$(Q)$(CC) -c -o $(1:.o=.d) $(2) $(CPPFLAGS) $(CFLAGS) $(3) \
	          -Xmake-dependency=1                        \
	          -Xmake-dependency-target="$(1)"
	$(Q)$(CC) -c -o $(1) $(2) $(CPPFLAGS) $(CFLAGS) $(3)
endef

# ------------------------------------------------------------------------------
# function compile_linkerfile
#    $(1)  <linker file destination>
#    $(2)  <linker file source>
#    $(3)  <flags>
#
# example:
#    $(call compile_linkerfile,build/file.dld,src/linker.dld,-DLINKER_DONT_FILL_ROM)
# ------------------------------------------------------------------------------
define compile_linkerfile
	$(call print_command,CC,$(2),>,$(1))
	$(Q)$(call ensure_dir, $(1))
	$(Q)$(CC) -Xpreprocessor-lineno-off $(3) -E $(2) > $(1)
endef

# ------------------------------------------------------------------------------
# function copy_file
#    $(1)  <destination>
#    $(2)  <source>
#
# example:
#    $(call copy_file,src/file1.c,dst/file2.c)
# ------------------------------------------------------------------------------
define copy_file
	$(call print_command,CP,$(2),,$(1),)
	$(Q)$(call ensure_dir,$(1))
	$(Q)cp $(2) $(1)
endef

# ------------------------------------------------------------------------------
# function create_gitstatus
#    $(1)  <target>
#    $(2)  <shellscript>
#
# example:
#    $(call create_gitstatus,gitstatus.c,src/mk/gitstatus.sh)
# ------------------------------------------------------------------------------
define create_gitstatus
	$(call print_command,GITBASH,,-o,$(1),)
	$(Q)$(call ensure_dir,$(1))
	$(Q)$(GITBASH) -c "$(2) $(1)"
endef

# ------------------------------------------------------------------------------
# function create_buildtype
#    $(1)  <target>
#    $(2)  <shellscript>
#    $(3)  <buildType>
#
# example:
#    $(call create_buildtype,BuildType.c,src/mk/buildtype.sh)
# ------------------------------------------------------------------------------
define create_buildtype
	$(call print_command,GITBASH,,-o,$(1) $(3),)
	$(Q)$(call ensure_dir,$(1))
	$(Q)$(GITBASH) -c "$(2) $(1) $(3)"
endef

# ------------------------------------------------------------------------------
# function create_linkinfo
#    $(1)  <target>
#    $(2)  <shellscript>
#
# example:
#    $(call create_linkinfo,linkinfo.c,src/mk/linkinfo.sh)
# ------------------------------------------------------------------------------
define create_linkinfo
	$(call print_command,LINKINFO,,-o,$(1),)
	$(Q)$(call ensure_dir,$(1))
	$(Q)$(GITBASH) -c "$(2) $(1)"
endef

# ------------------------------------------------------------------------------
# function delete_files
#    $(1)  <files>
#
# example:
#    $(call delete_files,build/ src/file.a)
# ------------------------------------------------------------------------------
define delete_files
	$(call print_command,RM,,-rf,$(1),)
	$(Q)rm -rf $(1)
endef

# ------------------------------------------------------------------------------
# function ensure_dir
#    $(1)  <file>
#
# example:
#    $(call ensure_dir,build/generated/main.c)
# ------------------------------------------------------------------------------
define ensure_dir
	if test ! \( -d $(patsubst %/,%,$(dir $(1))) \) ; then \
		mkdir -p $(patsubst %/,%,$(dir $(1))) ;            \
	fi
endef

# ------------------------------------------------------------------------------
# function generate_sconf
#    $(1)  <target file>
#    $(2)  <xml file>
#
# example:
#    $(call generate_sconf,build/generated/sconf.c,src/cfg/sconf_project.xml)
# ------------------------------------------------------------------------------
define generate_sconf
	$(call print_command,SCONFEXE,$(2),,$(1),)
	$(Q)$(call ensure_dir,$(1))
	$(Q)$(SCONFEXE) -c $(2)
endef

# ------------------------------------------------------------------------------
# function link_objects
#    $(1)  <target>
#    $(2)  <sources>
#    $(3)  <linker file>
#    $(4)  <map file>
#
# example:
#    $(call link_objects,file.elf,filea.o fileb.o,build/ld.dld,build/obj.map)
# ------------------------------------------------------------------------------
define link_objects
	$(call print_command,LD,,-o,$(1),)
	$(Q)$(call ensure_dir,$(1))
	$(Q)$(LD) -o $(1) $(2) $(LDFLAGS) -Wm$(3) > $(4)
endef

# ------------------------------------------------------------------------------
# function print_command
#    $(1)  <command>
#    $(2)  <source>
#    $(3)  <between flags>
#    $(4)  <destination>
#    $(5)  <end flags>
#
# example
#    $(call print_command,cc,file.c,-o,file.o,-Wall)
# ------------------------------------------------------------------------------
define print_command
	@printf "$(PRINT_COMMAND)$(1)$(PRINT_DEFAULT)\
	         $(PRINT_SOURCE)$(2)$(PRINT_DEFAULT)\
	         $(3)\
	         $(PRINT_DESTINATION)$(4)$(PRINT_DEFAULT)\
	         $(5)\n"
endef

# ------------------------------------------------------------------------------
# function strip_elf
#    $(1)  <target>
#    $(2)  <sources>
#
# example:
#    $(call strip_elf,file_stripped.elf,file_debug.elf)
# ------------------------------------------------------------------------------
define strip_elf
	$(call print_command,STRIP,,-o,$(1),)
	$(Q)$(call ensure_dir,$(1))
	$(Q)$(DDUMP) $(2) -T -o $(1)
endef
