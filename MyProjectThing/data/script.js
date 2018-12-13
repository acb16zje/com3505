$(document).ready(function () {

  let keyDict = { "ArrowLeft": false, "ArrowUp": false, "ArrowRight": false, "ArrowDown": false };
  let speed = 50;
  let isMobile = 'ontouchstart' in document.documentElement;

  // Keyboard listener
  $(document).keydown((e) => {
    switch (e.key) {
      case "ArrowLeft":
        setKeyDict("ArrowLeft", true);
        break;
      case "ArrowUp":
        setKeyDict("ArrowUp", true);
        break;
      case "ArrowRight":
        setKeyDict("ArrowRight", true);
        break;
      case "ArrowDown":
        setKeyDict("ArrowDown", true);
        break;
      default: return;
    }
    e.preventDefault();
  });

  $(document).keyup((e) => {
    switch (e.key) {
      case "ArrowLeft": case "ArrowUp": case "ArrowRight": case "ArrowDown":
        setKeyDict(e.key, false);
        break;
      default: return;
    }
    e.preventDefault();
  });

  if (isMobile) {
    // Mobile touch listener
    $('#left').on('touchstart', () => {
      setKeyDict("ArrowLeft", true);
    });
    $('#left').on('touchend', () => {
      setKeyDict("ArrowLeft", false);
    });

    $('#forward').on('touchstart', () => {
      setKeyDict("ArrowUp", true);
    });
    $('#forward').on('touchend', () => {
      setKeyDict("ArrowUp", false);
    });

    $('#right').on('touchstart', () => {
      setKeyDict("ArrowRight", true);
    });
    $('#right').on('touchend', () => {
      setKeyDict("ArrowRight", false);
    });

    $('#backward').on('touchstart', () => {
      setKeyDict("ArrowDown", true);
    });
    $('#backward').on('touchend', () => {
      setKeyDict("ArrowDown", false);
    });
  } else {
    // Desktop mouse listener
    $('#left').mousedown(() => {
      setKeyDict("ArrowLeft", true);
    });
    $('#left').mouseup(() => {
      setKeyDict("ArrowLeft", false);
    });

    $('#forward').mousedown(() => {
      setKeyDict("ArrowUp", true);
    });
    $('#forward').mouseup(() => {
      setKeyDict("ArrowUp", false);
    });

    $('#right').mousedown(() => {
      setKeyDict("ArrowRight", true);
    });
    $('#right').mouseup(() => {
      setKeyDict("ArrowRight", false);
    });

    $('#backward').mousedown(() => {
      setKeyDict("ArrowDown", true);
    });
    $('#backward').mouseup(() => {
      setKeyDict("ArrowDown", false);
    });
  }

  $('#speed').change(() => {
    callFunction('setSpeed', $('#speed').val());
  });

  function setKeyDict(k, v) {
    if (keyDict[k] != v) {
      Object.entries(keyDict).forEach(
        ([key, value]) => {
          keyDict[key] = key == k ? v : false;
        }
      );
      moveRobot();
    }
  }

  function moveRobot() {
    // Left
    if (keyDict.ArrowLeft) {
      $('#left').addClass('btn-primary');
      callFunction('left');
    } else {
      $('#left').removeClass('btn-primary');
    }

    // Forward
    if (keyDict.ArrowUp) {
      $('#forward').addClass('btn-primary');
      callFunction('forward');
    } else {
      $('#forward').removeClass('btn-primary');
    }

    // Right
    if (keyDict.ArrowRight) {
      $('#right').addClass('btn-primary');
      callFunction('right');
    } else {
      $('#right').removeClass('btn-primary');
    }

    // Backward
    if (keyDict.ArrowDown) {
      $('#backward').addClass('btn-primary');
      callFunction('backward');
    } else {
      $('#backward').removeClass('btn-primary');
    }

    // Stop
    if (!keyDict.ArrowLeft && !keyDict.ArrowUp && !keyDict.ArrowRight && !keyDict.ArrowDown) {
      callFunction('stop');
    }
  }

  // Control page
  $('#mode input').change(() => {
    let radioValue = $('input[name=mode]:checked', '#mode').val();
    callFunction('setMode', radioValue);

    $('#forward').prop('disabled', radioValue == "auto");
    $('#backward').prop('disabled', radioValue == "auto");
    $('#left').prop('disabled', radioValue == "auto");
    $('#right').prop('disabled', radioValue == "auto");
    $('#speed').prop('disabled', radioValue == "auto");

    if (radioValue == "auto") {
      $('#speedText').addClass("d-none");
      $('#speed').addClass('d-none');
      $('#recall').removeClass('d-none');
    } else {
      $('#speedText').removeClass("d-none");
      $('#speed').removeClass('d-none');
      $('#recall').addClass('d-none');
    }
  });

  $('#recall').click(function() {
    $(this).text('Recalling Robocar...');
    $(this).prop('disabled', true);
    callFunction('recall');
  });

  // WiFi list page
  $('#ap tr').click(function() {
    $(this).find('td input[type=radio]').prop('checked', true);
    document.getElementById('hidText').disabled = !document.getElementById('hidRadio').checked;
  })

  $('#hidRadio').click(() => {
    document.getElementById('hidText').disabled = !document.getElementById('hidRadio').checked;
  });

  // aREST function
  function callFunction(n,a,l){$.ajax({url:"http://192.168.4.1/"+n+"?params="+a}).done(function(n){null!=l&&l(n)})}
});