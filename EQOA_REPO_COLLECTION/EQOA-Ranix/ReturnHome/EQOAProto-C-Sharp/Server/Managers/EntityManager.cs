using System.Collections.Generic;
using ReturnHome.Server.EntityObject;

namespace ReturnHome.Server.Managers
{
    public static class EntityManager
    {
        private static List<Entity> entityList = new();
        private static ObjectIDCreator _idCreator;

        static EntityManager()
        {
            //Create an ObjectID Creator Instance for NPC's
            _idCreator = new ObjectIDCreator(true);
        }
        public static bool AddEntity(Entity entity)
        {
            if (!entity.isPlayer)
            {
                _idCreator.GenerateID(entity, out uint ObjectID);
                entity.ObjectID = ObjectID;
            }

            else
            {
                if (!_idCreator.AddEntity(entity))
                    return false;

            }

            //Add entity to our tracking List
            if (entityList.Contains(entity))
                //Return false here? Boot in world entity and load new one?
                return false;

            entityList.Add(entity);
            return true;
        }

        public static bool RemoveEntity(Entity entity)
        {
            if (!entityList.Contains(entity))
                return false;
            entityList.Remove(entity);
            return true;   
        }

        public static bool QueryForEntity(uint ObjectID, out Entity e)
        {
            if(_idCreator.QueryEntity(ObjectID, out Entity ent))
            {
                e = ent;
                return true;
            }

            e = default;
            return false;
        }

        public static bool QueryForEntity(string name, out Entity c)
        {
            foreach (Entity c2 in entityList)
            {
                if (c2.CharName == name)
                {
                    c = c2;
                    return true;
                }
            }
            c = default;
            return false;
        }

        public static List<Entity> QueryForAllEntitys()
        {
            return entityList;
        }
    }
}
