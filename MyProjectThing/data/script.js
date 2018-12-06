$(document).ready(function () {

  let keyDict = { "ArrowLeft": false, "ArrowUp": false, "ArrowRight": false, "ArrowDown": false };
  let baseSpeed = 40;
  let leftSpeed = rightSpeed = baseSpeed;
  let forwardTurnSpeed = baseSpeed / 2;

  // Keyboard listener
  $(document).keydown(function (e) {
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

  $(document).keyup(function (e) {
    switch (e.key) {
      case "ArrowLeft": case "ArrowUp": case "ArrowRight": case "ArrowDown":
        setKeyDict(e.key, false);
        break;
      default: return;
    }
    e.preventDefault();
  });

  // Mouse listener
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

  $('#speed').change(() => {
    baseSpeed = parseInt($('#speed').val());
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

  // aREST function
  function callFunction(n,a,l){$.ajax({type:"GET", url:"http://192.168.4.1/"+n+"?params="+a}).done(function(n){null!=l&&l(n)})}
});