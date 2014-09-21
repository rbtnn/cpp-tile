
let s:is_windows = has('win95') || has('win16') || has('win32') || has('win64')
if s:is_windows
  let s:repo_dir = fnamemodify(expand('<sfile>'), ":h:h")
  let s:tile_bin_path = s:repo_dir . '/tile.exe'
  let s:tile_config_path = s:repo_dir . '/tile.json'

  function! s:tile_start()
    if filereadable(s:tile_bin_path)
      execute printf('!start %s %s', s:tile_bin_path, s:tile_config_path)
    endif
  endfunction
  function! s:tile_edit_configurefile()
    execut 'edit ' . s:tile_config_path
  endfunction

  command! -nargs=0 TileStart silent call <sid>tile_start()
  command! -nargs=0 TileEditConfigureFile silent call <sid>tile_edit_configurefile()
endif

