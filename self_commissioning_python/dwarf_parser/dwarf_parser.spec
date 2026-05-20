# -*- mode: python -*-

block_cipher = None


a = Analysis(['dwarf_parser.py'],
             pathex=['C:\\Program Files (x86)\\Windows Kits\\10\\Redist\\ucrt\\DLLs\\x86', 'C:\\Data\\Projects\\autotesting_dev\\dwarf_parser'],
             binaries=[],
             datas=[],
             hiddenimports=[],
             hookspath=[],
             runtime_hooks=[],
             excludes=[],
             win_no_prefer_redirects=False,
             win_private_assemblies=False,
             cipher=block_cipher)
pyz = PYZ(a.pure, a.zipped_data,
             cipher=block_cipher)
exe = EXE(pyz,
          a.scripts,
          a.binaries,
          a.zipfiles,
          a.datas,
          name='dwarf_parser',
          debug=False,
          strip=False,
          upx=True,
          runtime_tmpdir=None,
          console=True )
