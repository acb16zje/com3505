$(document).ready(function () {

  // Device
  var device = new Device("192.168.4.1");

  let keyDict = {"ArrowLeft": false, "ArrowUp": false, "ArrowRight": false, "ArrowDown": false};
  let leftSpeed = rightSpeed = 50;
  let forwardTurnSpeed = 25;

  // Keyboard listener
  $(document).keydown(function (e) {
    switch (e.key) {
      case "ArrowLeft":
        setKeyDict("ArrowLeft", true);
        setKeyDict("ArrowRight", false);
        break;
      case "ArrowUp":
        setKeyDict("ArrowUp", true);
        setKeyDict("ArrowDown", false);
        break;
      case "ArrowRight":
        setKeyDict("ArrowRight", true);
        setKeyDict("ArrowLeft", false);
        break;
      case "ArrowDown":
        setKeyDict("ArrowDown", true);
        setKeyDict("ArrowUp", false);
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

  $(document).mouseup(() => {
    keyDict.ArrowLeft = keyDict.ArrowUp = keyDict.ArrowRight = keyDict.ArrowDown = false;
    moveRobot();
  })

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

  function setKeyDict(prop, val) {
    if (keyDict[prop] != val) {
      keyDict[prop] = val;
      moveRobot();
    }
  }

  function moveRobot() {
    // Left
    if (keyDict.ArrowLeft) {
      $('#left').addClass('btn-primary');
    } else {
      $('#left').removeClass('btn-primary');
    }

    // Forward
    if (keyDict.ArrowUp) {
      $('#forward').addClass('btn-primary');
    } else {
      $('#forward').removeClass('btn-primary');
    }

    // Right
    if (keyDict.ArrowRight) {
      $('#right').addClass('btn-primary');
    } else {
      $('#right').removeClass('btn-primary');
    }

    // Backward
    if (keyDict.ArrowDown) {
      $('#backward').addClass('btn-primary');
    } else {
      $('#backward').removeClass('btn-primary');
    }

    // Stop
    if (!keyDict.ArrowLeft && !keyDict.ArrowUp && !keyDict.ArrowRight && !keyDict.ArrowDown) {
      device.callFunction('stop');
    }

    if (keyDict.ArrowUp || keyDict.ArrowDown) {
      if (keyDict.ArrowLeft) {
        leftSpeed = 50;
        rightSpeed = 50 + forwardTurnSpeed;
      } else if (keyDict.ArrowRight) {
        leftSpeed = 50 + forwardTurnSpeed;
        rightSpeed = 50;
      } else {
        leftSpeed = rightSpeed = 50;
      }

      device.callFunction("setLeftSpeed", leftSpeed);
      device.callFunction("setRightSpeed", rightSpeed);

      if (keyDict.ArrowUp) {
        device.callFunction('forward');
      } else if (keyDict.ArrowDown) {
        device.callFunction('backward');
      }
    } else if (keyDict.ArrowLeft) {
      leftSpeed = rightSpeed = 50;
      device.callFunction("setLeftSpeed", leftSpeed);
      device.callFunction("setRightSpeed", rightSpeed);
      device.callFunction('left');
    } else if (keyDict.ArrowRight) {
      leftSpeed = rightSpeed = 50;
      device.callFunction("setLeftSpeed", leftSpeed);
      device.callFunction("setRightSpeed", rightSpeed);
      device.callFunction('right');
    }
  }
});

