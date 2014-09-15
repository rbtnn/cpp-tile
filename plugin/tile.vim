
let s:is_windows = has('win95') || has('win16') || has('win32') || has('win64')
if s:is_windows
  let s:repo_dir = fnamemodify(expand('<sfile>'), ":h:h")
  let s:tile_bin_path = s:repo_dir . '/tile.exe'
  let s:tile_ini_path = s:repo_dir . '/tile.ini'

  function! s:tile()
    if filereadable(s:tile_bin_path)
      execute '!start ' . s:tile_bin_path
    endif
  endfunction
  function! s:tile_edit_inifile()
    execut 'edit ' . s:tile_ini_path
  endfunction

  command! -nargs=0 Tile silent call <sid>tile()
  command! -nargs=0 TileEditIniFile silent call <sid>tile_edit_inifile()
endif

