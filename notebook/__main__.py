from __future__ import annotations

import subprocess
from logging import ERROR, basicConfig, getLogger
from pathlib import Path
from string import ascii_lowercase

import click

from notebook.expander import Expander

CXX_FLAGS = [
    "-std=c++20",
    "-O2",
    "-g",
    "-fsanitize=address",
    "-fsanitize=undefined",
    "-Wall",
    "-Wextra",
    "-Wpedantic",
    "-Wshadow",
    "-Wconversion",
]

TEMPLATE = """
#include <bits/stdc++.h>
#include <lib/prelude.hpp>

auto solve() {
}

auto main() -> i32 {
        std::ios_base::sync_with_stdio(false);
        std::cin.tie(nullptr);

        i32 t;
        std::cin >> t;

        while (t--) {
                solve();
        }
}
""".strip()

logger = getLogger()


@click.group()
def main() -> None:
    """Entrypoint."""


@main.command()
@click.argument("q")
@click.option("-d", "--debug", default=False, is_flag=True)
@click.option("-i", "--interactive", default=False, is_flag=True)
def run(q: str, debug: bool, interactive: bool) -> None:
    """
    Runner.

    release: Include line numbers in expansion
    """
    q_dir = Path.cwd() / "contest" / q

    src_path = (q_dir / q).with_suffix(".cpp")
    lib_paths = [Path.cwd()]

    expander = Expander(lib_paths)
    dst_code = expander.expand(src_path, show_lineno=debug)

    dst_path = (Path().cwd() / "contest" / "out").with_suffix(".cpp")
    dst_path.write_text(dst_code)

    logger.info("expanded: %s", q)

    exec_path = Path().cwd() / "build" / q
    input_path = q_dir / "input.txt"

    subprocess.run(
        ["g++", dst_path, *CXX_FLAGS, "-o", exec_path],
        check=False,
    )

    logger.info("compiled: %s", q)

    if interactive:
        subprocess.run([exec_path], check=False)
    else:
        with input_path.open("r") as f:
            subprocess.run([exec_path], stdin=f, check=False)


@main.command()
def generate() -> None:
    """Generate template."""
    for q in ascii_lowercase:
        q_dir = Path.cwd() / "contest" / q
        Path.mkdir(q_dir, exist_ok=True)

        src_path = (q_dir / q).with_suffix(".cpp")
        src_path.write_text(TEMPLATE)

        input_path = q_dir / "input.txt"
        input_path.write_text("")


basicConfig(
    format="%(asctime)s [%(levelname)s] %(message)s",
    datefmt="%Y-%m-%d %H:%M:%S",
    level=ERROR,
)

main()
