using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;

namespace WebAPI.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class ArduinoController : ControllerBase
    {

        private readonly HardwareService hardware;

        public ArduinoController(HardwareService hardware)
        {
            this.hardware = hardware;
        }
        [HttpGet("status")]
        public IActionResult Getstatus()
        {
            return Ok(hardware.CurrentState);
        }

        [HttpPost("unlock")]
        public IActionResult Unlock([FromBody] PasswordRequest request)
        {
            int attempts = 0;
            if (request.Password == "1234") // Your actual logic
            {
                hardware.SendToArduino("U"); // Send 'U' for Unlock to C code
                return Ok(new { message = "Access Granted" });
            }
            else
            {
                if (attempts >= 3)
                {
                    hardware.SendToArduino("P");
                }
                hardware.SendToArduino("F");
                return Unauthorized(new { message = "Wrong Password" });
            }
        }
        [HttpPost("panic")]
        public IActionResult Panic()
        {
            hardware.SendToArduino("P");
            return Ok(new { message = "Hardware Panic Initiated" });
        }
    }
    //dto needed to get the password from react 
    public class PasswordRequest
    {
        public string? Password { get; set; }
    }
}
