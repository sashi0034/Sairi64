import os

# ディレクトリ名を指定
dir_path = input("Please enter the directory path: ")

# 指定されたディレクトリ内のファイル名をすべて取得
files = os.listdir(dir_path)

# 拡張子を除く
filenames_without_ext = [os.path.splitext(f)[0] for f in files if os.path.isfile(os.path.join(dir_path, f))]

# 重複したファイル名を1つだけにする
unique_filenames = list(set(filenames_without_ext))

# ファイル名だけ列挙してprint
for name in unique_filenames:
    print(name)
