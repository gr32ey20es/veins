set nocompatible                    " be iMproved, required
filetype off                        " required

set rtp+=~/.vim/bundle/Vundle.vim   " runtime path to include Vundle & init
call vundle#begin()                 " call vundle#begin('~/some/path/here') 
" alternatively, pass a path where Vundle should install plugins

Plugin 'VundleVim/Vundle.vim'       " let Vundle manage Vundle, required

Plugin 'godlygeek/tabular'
Plugin 'preservim/vim-markdown'


" All of your Plugins must be added before the following line
call vundle#end()                   " required
filetype plugin on                  " required

" :PluginInstall    - installs plugins; append `!` to update or :PluginUpdate
" :PluginClean      - confirms removal of unused plugins; `!` to auto-approve 

set history=100
set autoread
autocmd FocusGained,BufEnter * silent! checktime

set enc=utf-8
set fenc=utf-8
set termencoding=utf-8

set showmatch
set autoindent
set smartindent

set tabstop=4
set shiftwidth=4
set expandtab

set textwidth=79
set colorcolumn=80
highlight ColorColumn ctermbg=8

set ignorecase
set smartcase

set hlsearch
set incsearch
highlight Search cterm=NONE ctermbg=8

set number
set cursorline
set cursorlineopt=number
highlight LineNr ctermfg=3
highlight CursorLineNr cterm=NONE ctermfg=8 

set wildmenu
set wildmode=longest:lastused:full
set completeopt-=menu

set whichwrap+=>,l
set whichwrap+=<,h

set path+=$PWD/**
set tags=./tags,tags;$HOME
set comments=sl:/*,mb:\ *,elx:\ */

set laststatus=2
highlight StatusLine ctermfg=7 ctermbg=0 cterm=standout,bold
highlight StatusLineNC ctermfg=7 ctermbg=8 cterm=bold,italic

let Tlist_Auto_Open=1
set updatetime=2000
