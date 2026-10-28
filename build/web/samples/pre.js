Module['preRun'] = Module['preRun'] || [];
Module['preRun'].push(function() {
  FS.mkdirTree('/tmp/.cache/dali_common_caches/shader/internal');
  FS.mkdirTree('/tmp/.cache/dali_common_caches/shader/custom');
  FS.mkdirTree('/tmp/fontconfig');
  ENV['FONTCONFIG_PATH'] = '/etc/fonts';
  ENV['FONTCONFIG_FILE'] = '/etc/fonts/fonts.conf';
  ENV['DALI_SHADER_USE_PROGRAM_BINARY'] = '0';
  ENV['DALI_DISABLE_PARTIAL_UPDATE'] = '1';
  // The editor uses a fixed logical scene on its resizable physical canvas.
  if (typeof document !== 'undefined') {
    var query = new URLSearchParams(location.search);
    if (query.get('embedded') === '1') {
      var bounded = function(value, fallback, minimum) {
        var number = Number(value);
        return Number.isFinite(number) && number >= (minimum || 64) && number <= 8192 ? Math.round(number) : fallback;
      };
      var bounds = Module.canvas.getBoundingClientRect();
      ENV['DALI_SCENE_SIZE_FOLLOWS_WINDOW'] = '0';
      ENV['DALI_SCENE_WIDTH'] = String(bounded(query.get('sceneWidth'), 800));
      ENV['DALI_SCENE_HEIGHT'] = String(bounded(query.get('sceneHeight'), 600));
      ENV['DALI_WINDOW_WIDTH'] = String(bounded(bounds.width * devicePixelRatio, 800, 1));
      ENV['DALI_WINDOW_HEIGHT'] = String(bounded(bounds.height * devicePixelRatio, 600, 1));
      ENV['DALI_WEB_RESIZE_TO_CANVAS'] = '1';
    }
  }
});
