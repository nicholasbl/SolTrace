#!/usr/bin/env python3
# pandoc_exporter.py
# Usage: python pandoc_exporter.py ./en template.tex output.pdf

import os
import sys
import subprocess

def parse_metadata(content):
    meta = {}
    if not content.startswith("---"):
        return meta, content
    end = content.find("---", 3)
    if end == -1:
        return meta, content
    frontmatter = content[3:end].strip()
    body = content[end+3:].strip()
    for line in frontmatter.splitlines():
        if ":" in line:
            k, _, v = line.partition(":")
            meta[k.strip().lower()] = v.strip().strip('"')
    return meta, body

def md_to_latex(text):
    result = subprocess.run(
        ["pandoc", "--from", "markdown", "--to", "latex"],
        input=text, capture_output=True, text=True
    )
    return result.stdout.strip()

def walk_docs(dir_path, key_prefix="", depth=0):
    vars = {}
    manifest_path = os.path.join(dir_path, "manifest.txt")
    if os.path.exists(manifest_path):
        with open(manifest_path) as f:
            entries = [l.strip() for l in f.readlines() if l.strip()]
    else:
        entries = sorted(f for f in os.listdir(dir_path) if f.endswith(".md"))
    for i, entry in enumerate(entries, 1):
        filepath = os.path.join(dir_path, entry)
        if not os.path.isfile(filepath):
            continue
        name = entry.replace(".md", "")
        new_key = name if not key_prefix else f"{key_prefix}.{name}"
        pandoc_key = new_key.replace(".", "_")
        with open(filepath) as f:
            content = f.read()
        meta, body = parse_metadata(content)
        if depth > 0:
            vars[pandoc_key] = md_to_latex(body)
            for mk, mv in meta.items():
                vars[f"{pandoc_key}-{mk}"] = md_to_latex(mv)
        subdir = os.path.join(dir_path, name)
        if os.path.isdir(subdir):
            vars.update(walk_docs(subdir, new_key, depth + 1))
    return vars

def build_pdf(docs_dir, template, output="output.pdf"):
    print("Walking docs...")
    vars = walk_docs(docs_dir)

    print("Building PDF via pandoc...")
    args = ["pandoc", "--from", "markdown", "--to", "pdf",
            "--template", template,
            "--output", output,
            "/dev/null"]
    for k, v in vars.items():
        args += ["--variable", f"{k}={v}"]

    result = subprocess.run(args, capture_output=True, text=True)
    if result.returncode == 0:
        print(f"Success! Output: {output}")
    else:
        print(f"Error:\n{result.stderr}")

if __name__ == "__main__":
    docs_dir = sys.argv[1] if len(sys.argv) > 1 else "./en"
    template  = sys.argv[2] if len(sys.argv) > 2 else "template.tex"
    output    = sys.argv[3] if len(sys.argv) > 3 else "output.pdf"
    build_pdf(docs_dir, template, output)
