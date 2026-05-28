using System;
using ReturnHome.Server.EntityObject;
using ReturnHome.Server.Managers;
using ReturnHome.Server.Network;
using ReturnHome.Server.Opcodes.Messages.Server;
using ReturnHome.Server.EntityObject.Stats;

namespace ReturnHome.Server.Opcodes.Chat
{
    public static class ObjectAdminChecks
    {
        public static bool ProcessChanges(Session MySession, string[] changes)
        {
            string message;
            /*Have this utilize players target as the source for change*/
            if(EntityManager.QueryForEntity(MySession.MyCharacter.Target, out Entity c))
            {
                if (c == null)
                    return false;

                message = $"Found character: {c.CharName}";
                ChatMessage.GenerateClientSpecificChat(MySession, message);

                switch(changes[1])
                {
                    case "Zone":
                        message = $"Map: {MySession.MyCharacter.map.Name}";
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "Lava":
                        ServerTeleportPlayer.TeleportPlayer(MySession, World.LavaStorm, 7906.09f, 75.2822f, 5914.65f, -2.40847f);
                        break;

                    case "Tun":
                        ServerTeleportPlayer.TeleportPlayer(MySession, World.Tunaria, 25273f, 54.125f, 15723.3f, -1.56333f);
                        break;

                    case "Arc":
                        ServerTeleportPlayer.TeleportPlayer(MySession, World.Odus, 7065.4609375f, 57.65724563598633f, 3499.9208984375f, -3.1183998584747314f);
                        break;

                    case "Rathe":
                        ServerTeleportPlayer.TeleportPlayer(MySession, World.RatheMountains, 8949.796875f, 54.125919342041016f, 7189.22314453125f, -1.4557989835739136f);
                        break;

                    case "pos":
                        ServerTeleportPlayer.TeleportPlayer(MySession, World.PlaneOfSky, 4823.9443359375f, 250.42288208007812f, 5504.09716796875f, -3.1277709007263184f);
                        break;

                    case "pod":
                        ServerTeleportPlayer.TeleportPlayer(MySession, World.Secrets, 5063.4736328125f, -24.68804931640625f, 3880.777587890625f, 3.1250364780426025f);
                        break;

                    case "zp":
                        ServerTeleportPlayer.TeleportPlayer(MySession, World.Secrets, 4872.84375f, 147.65725708007812f, 7047.947265625f, -1.588308334350586f);
                        break;

                    case "iod":
                        ServerTeleportPlayer.TeleportPlayer(MySession, World.Tunaria, 24568f, 53.5f, 3502f, -3.0826661586761475f);
                        break;

                    case "animation":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.Animation = byte.Parse(changes[2]);
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "size":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.ModelSize = float.Parse(changes[2]);
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "face":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.UpdateFacing(byte.Parse(changes[2]), 0);
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "face2":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.FacingF = float.Parse(changes[2]);
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "model":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.ModelID = int.Parse(changes[2]);
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "vy":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.VelocityY = float.Parse(changes[2]);
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "vx":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.VelocityX = float.Parse(changes[2]);
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "vz":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.VelocityZ = float.Parse(changes[2]);
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "move":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.Movement = byte.Parse(changes[2]);
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "ns":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.NorthToSouth = byte.Parse(changes[2]);
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "ew":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.EastToWest = byte.Parse(changes[2]);
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "lat":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.LateralMovement = byte.Parse(changes[2]);
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "turn":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.Turning = byte.Parse(changes[2]);
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "spin":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.SpinDown = byte.Parse(changes[2]);
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "namecolor":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.ObjectUpdateNameColor(byte.Parse(changes[2]));
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "nameplate":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.ObjectUpdateNamePlate(byte.Parse(changes[2]));
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "entity":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.ObjectUpdateEntity(byte.Parse(changes[2]));
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "test":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.ObjectUpdateUnknown();
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "status":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.ObjectUpdateStatus(byte.Parse(changes[2]));
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "currenthp":
                        message = $"Changing character: {c.CharName}, {changes[1]} to {changes[2]}";
                        c.CurrentHP = int.Parse(changes[2]);
                        ChatMessage.GenerateClientSpecificChat(MySession, message);
                        break;

                    case "stat":
                        switch(changes[2])
                        {
                            case "basestrength":
                                message = $"Changing character: {c.CharName}, {changes[2]} to {changes[3]}";
                                c.CurrentStats.dictionary[StatModifiers.BaseSTR] = int.Parse(changes[3]);
                                ChatMessage.GenerateClientSpecificChat(MySession, message);
                                break;

                            case "basestamina":
                                message = $"Changing character: {c.CharName}, {changes[2]} to {changes[3]}";
                                c.CurrentStats.dictionary[StatModifiers.BaseSTA] = int.Parse(changes[3]);
                                ChatMessage.GenerateClientSpecificChat(MySession, message);
                                break;

                            case "baseagility":
                                message = $"Changing character: {c.CharName}, {changes[2]} to {changes[3]}";
                                c.CurrentStats.dictionary[StatModifiers.BaseAGI] = int.Parse(changes[3]);
                                ChatMessage.GenerateClientSpecificChat(MySession, message);
                                break;

                            case "basedexterity":
                                message = $"Changing character: {c.CharName}, {changes[2]} to {changes[3]}";
                                c.CurrentStats.dictionary[StatModifiers.BaseDEX] = int.Parse(changes[3]);
                                ChatMessage.GenerateClientSpecificChat(MySession, message);
                                break;

                            case "basewisdom":
                                message = $"Changing character: {c.CharName}, {changes[2]} to {changes[3]}";
                                c.CurrentStats.dictionary[StatModifiers.BaseWIS] = int.Parse(changes[3]);
                                ChatMessage.GenerateClientSpecificChat(MySession, message);
                                break;

                            case "baseintelligence":
                                message = $"Changing character: {c.CharName}, {changes[2]} to {changes[3]}";
                                c.CurrentStats.dictionary[StatModifiers.BaseINT] = int.Parse(changes[3]);
                                ChatMessage.GenerateClientSpecificChat(MySession, message);
                                break;

                            case "basecharisma":
                                message = $"Changing character: {c.CharName}, {changes[2]} to {changes[3]}";
                                c.CurrentStats.dictionary[StatModifiers.BaseCHA] = int.Parse(changes[3]);
                                ChatMessage.GenerateClientSpecificChat(MySession, message);
                                break;

                            case "gearstrength":
                                message = $"Changing character: {c.CharName}, {changes[2]} to {changes[3]}";
                                c.CurrentStats.Add(StatModifiers.STR, int.Parse(changes[3]));
                                ChatMessage.GenerateClientSpecificChat(MySession, message);
                                break;

                            case "gearstamina":
                                message = $"Changing character: {c.CharName}, {changes[2]} to {changes[3]}";
                                c.CurrentStats.Add(StatModifiers.STA, int.Parse(changes[3]));
                                ChatMessage.GenerateClientSpecificChat(MySession, message);
                                break;

                            case "gearagility":
                                message = $"Changing character: {c.CharName}, {changes[2]} to {changes[3]}";
                                c.CurrentStats.Add(StatModifiers.AGI, int.Parse(changes[3]));
                                ChatMessage.GenerateClientSpecificChat(MySession, message);
                                break;

                            case "geardexterity":
                                message = $"Changing character: {c.CharName}, {changes[2]} to {changes[3]}";
                                c.CurrentStats.Add(StatModifiers.DEX, int.Parse(changes[3]));
                                ChatMessage.GenerateClientSpecificChat(MySession, message);
                                break;

                            case "gearwisdom":
                                message = $"Changing character: {c.CharName}, {changes[2]} to {changes[3]}";
                                c.CurrentStats.Add(StatModifiers.WIS, int.Parse(changes[3]));
                                ChatMessage.GenerateClientSpecificChat(MySession, message);
                                break;

                            case "gearintelligence":
                                message = $"Changing character: {c.CharName}, {changes[2]} to {changes[3]}";
                                c.CurrentStats.Add(StatModifiers.INT, int.Parse(changes[3]));
                                ChatMessage.GenerateClientSpecificChat(MySession, message);
                                break;

                            case "gearcharisma":
                                message = $"Changing character: {c.CharName}, {changes[2]} to {changes[3]}";
                                c.CurrentStats.Add(StatModifiers.CHA, int.Parse(changes[3]));
                                ChatMessage.GenerateClientSpecificChat(MySession, message);
                                break;

                            case "fr":
                                message = $"Changing character: {c.CharName}, {changes[2]} to {changes[3]}";
                                c.CurrentStats.Add(StatModifiers.FireResistance, int.Parse(changes[3]));
                                ChatMessage.GenerateClientSpecificChat(MySession, message);
                                break;
                            /*
                            case "unk2":
                                message = $"Changing character: {c.CharName}, {changes[2]} to {changes[3]}";
                                c.Unk2 = int.Parse(changes[3]);
                                ChatMessage.GenerateClientSpecificChat(MySession, message);
                                break;
                            */
                            default:
                                return false;

                        }
                        break;

                    default:
                        return false;
                }

                return true;
            }

            return false;
        }
    }
}
