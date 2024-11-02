from __future__ import annotations

import re
from logging import getLogger
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from pathlib import Path

logger = getLogger(__name__)

# #include <file_name.hpp>
INCLUDE_REGEX = re.compile(r"#include\s*<([a-z_/]*(.hpp))>\s*")

# #endif/ifndef/define (//) FILE_NAME_HPP
INCLUDE_GUARD_REGEX = re.compile(r"#.*[A-Z_]*_HPP")

STD_HEADERS = (
    "vector",
    "functional",
    "cstdint",
    "cassert",
    "algorithm",
    "utility",
    "iostream",
    "iomanip",
    "type_traits",
    "chrono",
    "string",
    "array",
    "bit",
    "random",
    "numeric",
    "queue",
    "stack",
    "set",
    "map",
)

STD_HEADER_REGEX = re.compile(rf"#include\s*<({'|'.join(STD_HEADERS)})>\s*")


class Expander:
    included: set[Path]
    lib_paths: list[Path]

    def __init__(self, lib_paths: list[Path]) -> None:
        self.included = set()
        self.lib_paths = lib_paths

    def ignore(self, line: str) -> bool:
        line = line.strip()

        return bool(
            INCLUDE_GUARD_REGEX.match(line)
            or STD_HEADER_REGEX.match(line)
            or line == "#pragma once"
            or line.startswith("//"),
        )

    def resolve(self, file_name: str) -> Path:
        for lib_path in self.lib_paths:
            file_path = lib_path / file_name
            if file_path.exists():
                return file_path

        logger.error("cannot find: %s", file_name)
        raise FileNotFoundError(file_name)

    def _expand(self, src_path: Path, show_lineno: bool) -> list[str]:
        result: list[str] = []
        if src_path in self.included:
            return result

        self.included.add(src_path)
        logger.info("include: %s", src_path)

        src_code = src_path.read_text()

        if show_lineno:
            result.append(f'#line 1 "{src_path}"')

        for lineno, line in enumerate(src_code.splitlines(), 1):
            if self.ignore(line):
                logger.info("ignoring: %s", line)
                continue

            if line == '#include <contest/debug.hpp>':
                result.append('')
                result.append('#ifdef NANDHAGK_LOCAL')
                result.append('#include "debug.hpp"')
                result.append('#else')
                result.append('#define debug(...)')
                result.append('#endif')

                continue

            if matches := INCLUDE_REGEX.match(line):
                file_name = matches.group(1)
                try:
                    file_path = self.resolve(file_name)

                    expanded = self._expand(file_path, show_lineno)

                    result.extend(expanded)
                    if show_lineno:
                        result.append(f'#line {lineno + 1} "{src_path}"')

                    continue
                except Exception:
                    pass

            result.append(line)

        return result

    def expand(self, src_path: Path, *, show_lineno: bool = True) -> str:
        self.included.clear()

        result = self._expand(src_path, show_lineno)
        return re.sub(r"(\n\s*)+\n+", "\n\n", "\n".join(result))
