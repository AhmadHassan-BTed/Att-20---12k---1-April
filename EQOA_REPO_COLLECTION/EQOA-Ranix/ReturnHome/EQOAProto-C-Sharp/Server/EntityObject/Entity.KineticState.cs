using System;
using QuadTrees.QTreePointF;
using System.Drawing;
using System.Numerics;
using ReturnHome.Utilities;
using ReturnHome.Server.Opcodes;
using ReturnHome.Server.EntityObject.Player;
using ReturnHome.Server.Zone;

namespace ReturnHome.Server.EntityObject
{
    public partial class Entity : IPointFQuadStorable
    {
        private World _world;

        public float x;
        public float y;
        public float z;
        public Map map;
        private float _velocityX = 0.0f;
        private float _velocityY = 0.0f;
        private float _velocityZ = 0.0f;

        //Set position to waypoint after initial position is set, should waypoint be assigned by the client update? Seems logical
        public Vector3 waypoint;
        private Vector3 _position;


        private byte _facing;
        private float _facingF;
        public float Speed;
        private byte _turning = 0;

        private byte _animation;
        public byte Movement = 1;

        private byte _eastToWest = 0;
        private byte _lateralMovement = 0;
        private byte _northToSouth = 0;
        private byte _spinDown = 0;
        private byte _firstPerson;

        //For QuadTree
        private PointF _point;

        public PointF Point
        {
            get { return _point; }
        }

        #region Property zone

        public World World
        {
            get { return _world; }
            set
            {
                if(value >= World.Tunaria && value <= World.Secrets)
                {
                    _world = value;
                    ObjectUpdateWorld();
                }
            }
        }

        public float VelocityX
        {
            get { return _velocityX; }
            set
            {
                if(true)
                {
                    _velocityX = value;
                    ObjectUpdateVelocityX();
                }
            }
        }

        public float VelocityY
        {
            get { return _velocityY; }
            set
            {
                if (true)
                {
                    _velocityY = value;
                    ObjectUpdateVelocityY();
                }
            }
        }

        public float VelocityZ
        {
            get { return _velocityZ; }
            set
            {
                if (true)
                {
                    _velocityZ = value;
                    ObjectUpdateVelocityZ();
                }
            }
        }

        public byte Facing
        {
            get { return _facing; }
            set
            {
                if(true)
                {
                    _facing = value;
                    ObjectUpdateFacing();
                }    
            }
        }

        public float FacingF
        {
            get { return _facingF; }
            set
            {
                if(true)
                {
                    _facingF = value;
                    ObjectUpdateFacingF();
                }
            }
        }

        public Vector3 Position
        {
            get { return _position; }
            set
            {
                if(true)
                {
                    _position = value;
                    x = _position.X;
                    y = _position.Y;
                    z = _position.Z;
                    _point = new PointF(_position.X, _position.Z);
                    ObjectUpdatePosition();
                }
            }
        }
        public byte EastToWest
        {
            get { return _eastToWest; }
            set
            {
                if(true)
                {
                    _eastToWest = value;
                    ObjectUpdateEastWest();
                }
            }
        }

        public byte LateralMovement
        {
            get { return _lateralMovement; }
            set
            {
                if(true)
                {
                    _lateralMovement = value;
                    ObjectUpdateLateralMovement();
                }
            }
        }

        public byte NorthToSouth
        {
            get { return _northToSouth; }
            set
            {
                if(true)
                {
                    _northToSouth = value;
                    ObjectUpdateNorthSouth();
                }
            }
        }

        public byte Turning
        {
            get { return _turning; }
            set
            {
                if(true)
                {
                    _turning = value;
                    ObjectUpdateTurning();
                }
            }
        }

        public byte SpinDown
        {
            get { return _spinDown; }
            set
            {
                if(true)
                {
                    _spinDown = value;
                    ObjectUpdateSpinDown();
                }
            }
        }

        public byte Animation
        {
            get { return _animation; }
            set
            {
                if(true)
                {
                    _animation = value;
                    ObjectUpdateAnimation();
                }
            }
        }

        public byte FirstPerson
        {
            get { return _firstPerson; }
            set
            {
                if(true)
                {
                    _firstPerson = value;
                    ObjectUpdateFirstPerson();
                }
            }
        }

        #endregion

        //Calculates our speed? I suspect this will be kinda wrong depending on network latency and such...
        public void CalculateSpeed()
        {
            /*
            _newestPosition = new Vector2(x, z);
            Speed = Vector2.Distance(Position, _newestPosition);
            Console.WriteLine($"Speed is {Speed}");
            Position = _newestPosition;
            */
        }

        public void CalculateDirection()
        {

        }

        public void UpdatePosition(float X, float Y, float Z)
        {
            Position = new Vector3(X, Y, Z);
        }

        public void UpdateFacing(byte facing, byte turning)
        {
            Facing = (byte)(facing + 128);
            FacingF = CoordinateConversions.ConvertFacing(Facing);
            Turning = turning;
        }

        /*
         * No need for an animation update method as the property can handle it all?
        public void UpdateAnimation(byte animation)
        {
            Animation = animation;
        }
        */

        public void UpdateVelocity(float velocityX, float velocityY, float velocityZ)
        {
            VelocityX = velocityX;
            VelocityY = velocityY;
            VelocityZ = velocityZ;
        }

        /*
         * No point for a Target update method as the property can handle everything?
        public void UpdateTarget(uint target)
        {
            Target = target;
        }

        public Vector3 getPosition()
        {
            if (movement_started == 0)
            {
                return Position;
            }

            float delay = (DateTimeOffset.Now.ToUnixTimeMilliseconds() - movement_started) / 1000.0f;

            Vector3 d = Vector3.Subtract(waypoint, Position);

            float dd = Vector3.Distance(waypoint, Position);

            if (dd > 0)
            {
                d.X = Speed * d.X / dd * delay;
                d.Y = Speed * d.Y / dd * delay;
                d.Z = Speed * d.Z / dd * delay;
            }

            Position = Vector3.Add(Position, d);

            return Position;
        }
        */
    }
}
