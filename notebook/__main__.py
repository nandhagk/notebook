from __future__ import annotations

import subprocess
from logging import basicConfig, getLogger, INFO
from pathlib import Path
from string import ascii_lowercase
from time import perf_counter

import click

from notebook.expander import Expander

CXX_FLAGS = [
    "-std=c++20",
    "-O2",
    "-g",
    "-DNANDHAGK_LOCAL",
    "-fsanitize=address",
    "-fsanitize=undefined",
    "-Wall",
    "-Wextra",
    "-Wshadow",
    "-Wconversion",
]

CXX_FLAGS_FAST = [
    "-std=c++20",
    "-Ofast",
    "-DNANDHAGK_LOCAL",
]

TEMPLATE = """
#include <bits/stdc++.h>
#include <contest/debug.hpp>

#include <lib/prelude.hpp>

void solve() {
}

i32 main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    i32 t;
    std::cin >> t;

    while (t--) solve();
}
""".strip()

logger = getLogger()


@click.group()
def main() -> None:
    """Entrypoint."""


@main.command()
@click.argument("q")
@click.option("-d", "--debug", default=False, is_flag=True)
@click.option("-f", "--fast", default=False, is_flag=True)
def run(q: str, debug: bool, fast: bool) -> None:
    """
    Runner.

    release: Include line numbers in expansion
    """
    q_dir = Path.cwd() / "contest" / q

    src_path = (q_dir / q).with_suffix(".cpp")
    lib_paths = [Path.cwd()]

    expander = Expander(lib_paths)

    t0 = perf_counter()
    dst_code = expander.expand(src_path, show_lineno=debug)

    dst_path = (Path().cwd() / "contest" / "out").with_suffix(".cpp")
    dst_path.write_text(dst_code)

    subprocess.run(["clang-format", dst_path, "-i"], check=False)

    t1 = perf_counter()
    logger.info("expanded %s in %dms", q, round((t1 - t0) * 1000))

    exec_path = Path().cwd() / "build" / q
    input_path = q_dir / "input.txt"

    t0 = perf_counter()
    subprocess.run(
        ["g++", dst_path, *(CXX_FLAGS_FAST if fast else CXX_FLAGS), "-o", exec_path],
        check=False,
    )

    t1 = perf_counter()
    logger.info("compiled %s in %dms", q, round((t1 - t0) * 1000))

    t0 = perf_counter()
    with input_path.open("r") as f:
        subprocess.run([exec_path], stdin=f, check=False)
    t1 = perf_counter()
    logger.info("ran %s in %dms", q, round((t1 - t0) * 1000))


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
    level=INFO,
)

main()
