import struct

SH_TYPE = {0: "SHT_NULL", 1: "SHT_PROGBITS", 2: "SHT_SYMTAB", 3: "SHT_STRTAB", 4: "SHT_RELA", 5: "SHT_HASH",
           6: "SHT_DYNAMIC", 7: "SHT_NOTE", 8: "SHT_NOBITS", 9: "SHT_REL", 10: "SHT_SHLIB", 11: "SHT_DYNSYM",
           14: "SHT_INIT_ARRAY", 15: "SHT_FINI_ARRAY", 16: "SHT_PREINIT_ARRAY", 17: "SHT_GROUP",
           18: "SHT_SYMTAB_SHNDX", 19: "SHT_RELR", 20: "SHT_NUM", 0x60000000: "SHT_LOOS",
           0x6ffffff5: "SHT_GNU_ATTRIBUTES", 0x6ffffff6: "SHT_GNU_HASH", 0x6ffffff7: "SHT_GNU_LIBLIST",
           0x6ffffffd: "SHT_GNU_verdef", 0x6ffffffe: "SHT_GNU_verneed", 0x6fffffff: "SHT_GNU_versym",
           0x80000000: "SHT_LOUSER", 0xffffffff: "SHT_HIUSER", 0x6ffffff3: "SHT_SUNW_LDYNSYM",
           0x6ffffffc: "SHT_SUNW_syminfo", 0x70000001: "SHT_ARM_EXIDX", 0x70000002: "SHT_ARM_PREEMPTMAP",
           0x70000003: "SHT_ARM_ATTRIBUTES", 0x70000004: "SHT_ARM_DEBUGOVERLAY"}
ST_BIND = {0: "STB_LOCAL", 1: "STB_GLOBAL", 2: "STB_WEAK", 3: "STB_NUM", 10: "STB_LOOS", 12: "STB_HIOS",
           13: "STB_LOPROC", 15: "STB_HIPROC"}
ST_TYPE = {0: "STT_NOTYPE", 1: "STT_OBJECT", 2: "STT_FUNC", 3: "STT_SECTION", 4: "STT_FILE", 5: "STT_COMMON",
           6: "STT_TLS", 7: "STT_NUM", 8: "STT_RELC", 9: "STT_SRELC", 10: "STT_LOOS", 12: "STT_HIOS",
           13: "STT_LOPROC", 15: "STT_HIPROC"}
ST_SHNDX = {0: "SHN_UNDEF", 0xfff1: "SHN_ABS", 0xfff2: "SHN_COMMON"}
ST_VISIBILITY = {0: "STV_DEFAULT", 1: "STV_INTERNAL", 2: "STV_HIDDEN", 3: "STV_PROTECTED", 4: "STV_EXPORTED",
                 5: "STV_SINGLETON", 6: "STV_ELIMINATE"}
SHF_COMPRESSED = 0x800
HEADER_KEYS = ("sh_name", "sh_type", "sh_flags", "sh_addr", "sh_offset", "sh_size", "sh_link", "sh_info",
               "sh_addralign", "sh_entsize")


class Symbol:
    __slots__ = ("entry", "name")

    def __init__(self, entry, name):
        self.entry = entry
        self.name = name

    def __getitem__(self, key):
        return self.entry[key]


class Relocation:
    __slots__ = ("entry",)

    def __init__(self, entry):
        self.entry = entry

    def is_RELA(self):
        return "r_addend" in self.entry

    def __getitem__(self, key):
        return self.entry[key]


class Section:
    def __init__(self, elf, header, name):
        self.elf = elf
        self.header = header
        self.name = name

    def __getitem__(self, key):
        return self.header[key]

    def data(self):
        if self.header["sh_type"] == "SHT_NOBITS":
            return b"\0" * self.header["sh_size"]
        off = self.header["sh_offset"]
        return bytes(self.elf.raw[off:off + self.header["sh_size"]])

    def is_null(self):
        return False


class StringTableSection(Section):
    def get_string(self, offset):
        start = self.header["sh_offset"] + offset
        end = self.elf.raw.find(b"\0", start)
        s = self.elf.raw[start:end if end >= 0 else len(self.elf.raw)]
        return s.decode("utf-8", errors="replace") if s else ""


class SymbolTableSection(Section):
    def __init__(self, elf, header, name, stringtable):
        super().__init__(elf, header, name)
        self.stringtable = stringtable
        self._symbols = None

    def num_symbols(self):
        return self.header["sh_size"] // self.header["sh_entsize"]

    def _all(self):
        if self._symbols is None:
            off, ent, raw = self.header["sh_offset"], self.header["sh_entsize"], self.elf.raw
            out = []
            for k in range(self.num_symbols()):
                st_name, value, size, info, other, shndx = struct.unpack_from("<IIIBBH", raw, off + k * ent)
                entry = {"st_name": st_name, "st_value": value, "st_size": size,
                         "st_info": {"bind": ST_BIND.get(info >> 4, info >> 4), "type": ST_TYPE.get(info & 0xf, info & 0xf)},
                         "st_other": {"local": other >> 5, "visibility": ST_VISIBILITY.get(other & 7, other & 7)},
                         "st_shndx": ST_SHNDX.get(shndx, shndx)}
                out.append(Symbol(entry, self.stringtable.get_string(st_name)))
            self._symbols = out
        return self._symbols

    def get_symbol(self, n):
        return self._all()[n]

    def iter_symbols(self):
        return iter(self._all())


class RelocationSection(Section):
    def is_RELA(self):
        return self.header["sh_type"] == "SHT_RELA"

    def num_relocations(self):
        return self.header["sh_size"] // (12 if self.is_RELA() else 8)

    def get_relocation(self, n):
        off = self.header["sh_offset"]
        if self.is_RELA():
            r_offset, r_info, r_addend = struct.unpack_from("<IIi", self.elf.raw, off + 12 * n)
            return Relocation({"r_offset": r_offset, "r_info": r_info, "r_info_sym": r_info >> 8,
                               "r_info_type": r_info & 0xff, "r_addend": r_addend})
        r_offset, r_info = struct.unpack_from("<II", self.elf.raw, off + 8 * n)
        return Relocation({"r_offset": r_offset, "r_info": r_info, "r_info_sym": r_info >> 8,
                           "r_info_type": r_info & 0xff})

    def iter_relocations(self):
        for n in range(self.num_relocations()):
            yield self.get_relocation(n)


class NullSection(Section):
    def is_null(self):
        return True


class _Elf:
    def __init__(self, raw, headers):
        self.raw = raw
        self._headers = headers
        self._sections = [None] * len(headers)
        self._names = None

    def num_sections(self):
        return len(self._headers)

    def _name(self, header):
        strtab = self._section_names_table
        return strtab.get_string(header["sh_name"]) if strtab is not None else ""

    def get_section(self, n):
        if self._sections[n] is None:
            header = self._headers[n]
            name = self._name(header)
            kind = header["sh_type"]
            if kind == "SHT_STRTAB":
                sec = StringTableSection(self, header, name)
            elif kind == "SHT_NULL":
                sec = NullSection(self, header, name)
            elif kind == "SHT_SYMTAB":
                sec = SymbolTableSection(self, header, name, self.get_section(header["sh_link"]))
            elif kind in ("SHT_REL", "SHT_RELA"):
                sec = RelocationSection(self, header, name)
            else:
                sec = Section(self, header, name)
            self._sections[n] = sec
        return self._sections[n]

    def iter_sections(self, type=None):
        for n in range(len(self._headers)):
            sec = self.get_section(n)
            if type is None or sec.header["sh_type"] == type:
                yield sec

    def get_section_by_name(self, name):
        if self._names is None:
            self._names = {sec.name: n for n, sec in enumerate(self.iter_sections())}
        n = self._names.get(name)
        return None if n is None else self.get_section(n)


def _parse(raw):
    if len(raw) < 52 or raw[:4] != b"\x7fELF" or raw[4] != 1 or raw[5] != 1:
        return None
    e_type, e_machine = struct.unpack_from("<HH", raw, 16)
    e_shoff, = struct.unpack_from("<I", raw, 0x20)
    e_shentsize, e_shnum, e_shstrndx = struct.unpack_from("<HHH", raw, 0x2E)
    if e_machine != 40 or e_shoff == 0 or e_shnum == 0 or e_shentsize != 40 or e_shstrndx >= e_shnum:
        return None
    headers = []
    for n in range(e_shnum):
        values = struct.unpack_from("<10I", raw, e_shoff + n * e_shentsize)
        header = dict(zip(HEADER_KEYS, values))
        if header["sh_flags"] & SHF_COMPRESSED or header["sh_type"] in (0x6ffffff3, 11, 18, 19):
            return None
        header["sh_type"] = SH_TYPE.get(header["sh_type"], header["sh_type"])
        headers.append(header)
    elf = _Elf(raw, headers)
    shstr = headers[e_shstrndx]
    elf._section_names_table = StringTableSection(elf, shstr, "") if shstr["sh_type"] == "SHT_STRTAB" else None
    if elf._section_names_table is None:
        return None
    for header in headers:
        if header["sh_type"] == "SHT_SYMTAB" and (header["sh_entsize"] != 16 or header["sh_size"] % 16):
            return None
    return elf


def ELFFile(stream):
    stream.seek(0)
    raw = stream.read()
    elf = _parse(raw)
    if elf is None:
        import io
        from elftools.elf.elffile import ELFFile as Full
        return Full(io.BytesIO(raw))
    return elf
