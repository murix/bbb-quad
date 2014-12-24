using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace beaglebone_quadcopter
{
    public static class MathHelper2
    {
        public static float Saturate(float value)
        {
            return value < 0 ? 0 : value > 1 ? 1 : value;
        }
    }

}
