
let s:is_windows = has('win95') || has('win16') || has('win32') || has('win64')
if s:is_windows
  let s:repo_dir = fnamemodify(expand('<sfile>'), ":h:h")
  let s:tile_bin_path = s:repo_dir . '/bin/tile.exe'
  let s:tile_ini_path = expand('~/tile.ini')
  let s:tile_makefile_path = expand('~/makefile')

  function! s:tile_start()
    if filereadable(s:tile_bin_path)
      execute '!start ' . s:tile_bin_path
    endif
  endfunction
  function! s:tile_open_repository()
    if isdirectory(s:repo_dir)
      execute printf('!start explorer.exe "%s"', s:repo_dir)
    endif
  endfunction
  function! s:tile_edit_inifile()
    execut 'edit ' . s:tile_ini_path 
  endfunction
  function! s:tile_make()
    let curr = getcwd()
    try
      execute 'lcd ' . s:repo_dir
      make
    finally
      execute 'lcd ' . curr
    endtry
  endfunction

  command! -nargs=0 TileStart silent call <sid>tile_start()
  command! -nargs=0 TileOpenRepository silent call <sid>tile_open_repository()
  command! -nargs=0 TileEditIniFile silent call <sid>tile_edit_inifile()
  command! -nargs=0 TileMake silent call <sid>tile_make()
endif

