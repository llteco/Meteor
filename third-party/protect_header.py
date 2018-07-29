#! coding=utf-8
# Add header protector to each headers
# @Author: Wenyi Tang
# @Email: wenyi.tang@intel.com

from pathlib import Path
import re
import argparse


def HasSeparator(line):
    """To tell if a separator ends in line"""

    if line[-1] == '\n':
        return True
    return False


def CollectHeadersFromDir(path, recurse=False):
    """Glob all headers (recursively) in the given path"""

    path = Path(path).absolute().resolve()
    if not path.exists():
        return None
    headers = list(path.glob('*.h'))
    if recurse:
        headers = list(path.rglob('*.h'))
    return headers


def GernerateProtectDefine(file, prefix='', root=None):
    """Generate protector from the parent path of file"""

    file = Path(file)
    root = Path(root) if root else Path.cwd()
    root = root.absolute().resolve()
    if not file.is_file():
        raise ValueError('must specify a file')
    if not root.exists():
        raise ValueError('invalid root, which must exist')
    # cut file.parent to root
    file = file.relative_to(root)
    path = str(file.parent).upper().replace('.', '')
    name = str(file.stem).upper()
    sep = '/' if '/' in path else '\\'
    path = path.replace(sep, '_')
    prefix = prefix + '_' if prefix else ''
    protector = f'{prefix}{path}_{name}_H_' if path else f'{prefix}{name}_H_'
    return protector


def FindFirstCodeLine(file):
    """Find the 1st line number which is not the comment in file"""

    line_no = 0
    with open(file, 'r', encoding='utf-8') as fd:
        comment_stack = []
        double_slash = re.compile('^\s*//')  # -> //
        slash_star = re.compile('^\s*/\*')  # -> /*
        star_slash = re.compile('\*/\s*')  # -> */
        empty_line = re.compile('^\s*$')
        while True:
            line = fd.readline()
            if not line:
                break
            line_no += 1
            if re.findall(empty_line, line):
                continue
            if re.findall(slash_star, line):
                comment_stack.append(line_no)
            if re.findall(double_slash, line):
                if not comment_stack:
                    continue
            ss = re.findall(star_slash, line)
            if ss:
                comment_stack.pop()
                continue
            if not comment_stack:
                # the 1st code line
                line_no -= 1
                break
    return line_no


def AddProtector(file, pos, protect, line_sep='\n'):
    # stash the file content
    content = []
    line_no = 0
    with open(file, 'r', encoding='utf-8') as fd:
        if_stack = []
        line = fd.readline()
        while line:
            # avoid duplicated protector
            if '#ifndef' in line and line_no == pos:
                # Do have a `#ifndef` semantic, parse the var
                var = line[8:-1]
                if var == protect:
                    return
                if_stack.append((var, line_no, line))
            if '#define' in line and if_stack:
                var = line[8:-1]
                if var == if_stack[-1][0]:
                    # `#ifndef` & `#define` semantics
                    if_stack.append((var, line_no, line))
            content.append(line)
            line = fd.readline()
            line_no += 1
        # remove empty lines at tail
        empty_line = re.compile('^\s*$')
        if content:
            while re.findall(empty_line, content[-1]):
                content.pop()
        if if_stack:
            assert len(if_stack) == 2
            if '#endif' in content[-1]:
                # remove the wrong protector
                content.remove(if_stack[0][2])
                content.remove(if_stack[1][2])
                content.pop()
    # restore the content with protector
    line_no = 0
    with open(file, 'w', encoding='utf-8') as fd:
        while line_no <= len(content):
            if pos == line_no:
                fd.write(f'#ifndef {protect}{line_sep}')
                fd.write(f'#define {protect}{line_sep}')
            if line_no < len(content):
                if HasSeparator(content[line_no]):
                    fd.write(content[line_no])
                else:
                    fd.write(content[line_no] + line_sep)
            line_no += 1
        if line_no >= len(content):
            fd.write(f'#endif  // {protect}{line_sep}')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('dir')
    parser.add_argument('--prefix', type=str, default='')
    parser.add_argument('--root', type=str, default=None)
    param = parser.parse_args()

    headers = CollectHeadersFromDir(param.dir, True)
    for f in headers:
        protector = GernerateProtectDefine(f, param.prefix, param.root)
        pos = FindFirstCodeLine(f)
        AddProtector(f, pos, protector)
