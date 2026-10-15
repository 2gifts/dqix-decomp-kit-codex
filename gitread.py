import re
import subprocess

DIFF_FILE = re.compile(r"^diff --git a/(.+) b/(.+)$")


def text(data):
    return data.decode("utf-8", errors="replace").replace("\r\n", "\n").replace("\r", "\n")


def blobs(repo, specs):
    specs = [s for s in dict.fromkeys(specs) if "\n" not in s]
    if not specs:
        return {}
    data = subprocess.run(["git", "-C", repo, "cat-file", "--batch"], input=("\n".join(specs) + "\n").encode("utf-8"),
                          capture_output=True).stdout
    out, i = {}, 0
    for spec in specs:
        nl = data.index(b"\n", i)
        head = data[i:nl].split()
        i = nl + 1
        if len(head) != 3 or not head[2].isdigit():
            out[spec] = ""
            continue
        size = int(head[2])
        out[spec] = text(data[i:i + size])
        i += size + 1
    return out


def diff_lines(repo, revs, paths=()):
    raw = subprocess.run(["git", "-C", repo, "diff", "-U0", "--no-color", "--no-ext-diff", "--no-renames", *revs,
                          "--", *paths], capture_output=True).stdout
    out, path, hunk = {}, None, False
    for line in text(raw).split("\n"):
        m = DIFF_FILE.match(line)
        if m:
            path, hunk = m.group(2), False
            out.setdefault(path, ([], []))
        elif line.startswith("@@"):
            hunk = True
        elif hunk and path and line[:1] in "+-" and line:
            out[path][line[0] == "-"].append(line[1:])
    return out
