colorscheme elflord

" cmd
set wildmenu
set cmdheight=2
set showcmd

" search
set hlsearch
set ignorecase
set incsearch
set smartcase 

" noswap
set noswapfile
set nobackup
set nowb

" indent
set smartindent
set autoindent
set smarttab
set shiftwidth=4
set softtabstop=4
set tabstop=4
set expandtab

" folding
set foldmethod=indent
set foldnestmax=3
set nofoldenable

" misc
set backspace=indent,eol,start
"set visualbell
set autoread
set cursorline
set nostartofline
set mouse=a
set number





let mapleader=","
nnoremap <leader><ENTER> :nohl<ENTER>

" swap between vs tabs
map <TAB> <C-W><C-W> 
map // :s:^://<CR>
map \\ :s:^//:<CR>

