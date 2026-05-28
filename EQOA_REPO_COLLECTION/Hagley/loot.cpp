#include "loot.h"
#include "merchants.h"
#include "objectpacket.h"
#include "generator.h"
#include "merchants.h"
#include "combat.h"
#include "charcreate.h"
#include "facingdirection.h"
#include "quests.h"
#include "maploading.h"
#include "fortytwentyninedec.h"
#include "packetparsing.h"
#include "opcodes.h"
#include "add_or_sub.h"
#include "convert.h"
#include "increment.h"
#include "flipped.h"
#include "coaching.h"

QMap <QString, QVector <QString>> Loot_List_Map;

QMap<QString,QMap<int,QString>> maploading::NPCsID_Flag_MenusID_Map;
QMap<QString,QString> maploading::MenuText_MenuIDMap;
QMap<QString,QMap<int,QVector<QString>>> maploading::NPCsID_Flag_CheckVectorMap;
QMap<QString,QMap<int,QVector<QString>>> maploading::NPCsID_Flag_ItemCheckVectorMap;
QMap<QString,QMap<int,QString>> maploading::NPCsID_Flag_TunarCheckMap;
QVector<int> maploading::Sheet_IntVector;
QVector<QString> maploading::ColumnTextVector;
bool maploading::MapLoadingFinished = false;
bool maploading::Excel_is_Loaded = false;
QMap<int,QVector<QString>> maploading::Row_Columns_Text_VectorMap;
QMap<int,QVector<QString>> maploading::Row_Menu_Options_VectorMap;
QMap<QString,QVector<QString>> maploading::Option_PopuptextVector;
QMap<int,QMap<QString,QVector<QString>>> maploading::Count_Option_PopuptextVector;
QMap<int,QVector<QTableWidgetItem*>> maploading::SpreadSheet_RowitemsVectorMap;
QVector<QString> maploading::MaploadingToUIvector;

packetparsing::packetvars packetparsing::pX;

QMap<QByteArray,struct packetparsing::packetvars> packetparsing::IPandPort_AllvariablesMap;

QVector<QByteArray> fortytwentyninedec::UpdateMapsVector;

QMap<QByteArray,QByteArray> FacingDirection::LoS_StatusMap;
QString FacingDirection::Now_Roaming;
QMap <QString,int> FacingDirection::RandomIDsMAP;
QMap<QByteArray,QByteArray> FacingDirection::HPxorMap;
QMap <QString,int> FacingDirection::Path_CounterMap;
QMap <QString,QMap<int,QString>> FacingDirection::Path_Maps;
QString FacingDirection::UseThisMap;
QMap <QString,QMap<int,QString>> FacingDirection::TempPathMaps;
QMap <QString,QByteArray> FacingDirection::Facing_Lock_Map;
QMap <QString,QByteArray> FacingDirection::Facing_Lock2_Map;
QString FacingDirection::AggroDistance;
float FacingDirection::AggroRunDistance;
float FacingDirection::AggroSpeed1 = 40.0;
QMap<QByteArray,QPixmap> FacingDirection::CurrentImageMap;

QMap<QString,QString> CharCreate::ID_StartingCityMap;
QMap<QString,QVector<QByteArray>> CharCreate::CharID_clientID_clientIP_clientPORTvectorMap;
QMap<QString,QVector<QString>> CharCreate::GroupID_PlayersID_NamesVectorMap;
QMap<QString,QVector<QString>> CharCreate::PendingGroupID_PlayersID_NamesVectorMap;
QMap<QString,QString> CharCreate::PlayersID_GroupID_Map;

QMap<QString,int> merchants::MerchantIDs_PacketIDsMap;
QMap <QString,QVector<QString>> merchants::MasterItems_Vector_Map;
QMap<QString,QString> merchants::Items_Map;
QMap <QString,QVector<QString>> merchants::Merchant_List_Map;
QMap <QString,QString> merchants::Merchant_Text_Map;

QMap<QByteArray,QByteArray> generator::Players_Map;

QMap <QString,QVector<QString>> objectpacket::IDs_DamageVectorMap;
QMap<QByteArray,QByteArray> objectpacket::Master_Map;
QMap<QByteArray,QByteArray> objectpacket::MasterMap2;
QVector <QString> objectpacket::DeadListVector;
QMap <QString,QVector <QString>> objectpacket::DeadListVectorMap;
QStringList objectpacket::holdx;
QStringList objectpacket::holdz;
QStringList objectpacket::holdy;
QStringList objectpacket::holdid1;
QByteArray objectpacket::PET_Exists = "00";
QMap<QString,QString>objectpacket::AGGRO_List;
QMap<QString,QString>objectpacket::Roamers_List;
QString objectpacket::FarthestID = "";
float objectpacket::AGGRO_Radius = 0.0;
float objectpacket::Roamers_Radius = 0.0;
QString objectpacket::Current_Chan;
QString objectpacket::AGGRO_Chan;
QString objectpacket::Roamers_Chan;
QString objectpacket::Resend_AGGRO_ID;
QByteArray objectpacket::Resend_AGGRO_Chan = "00";
QString objectpacket::AGGRO_New_Chan;
float objectpacket::distancecheck1 = 0.0;
QMap<QString,QString> objectpacket::DespawnDelete_Map;
QMap <QString,QVector <QString>> objectpacket::MonsterRaceVectorMap;
QMap <QString,QVector <QString>> objectpacket::objectpacket::MonsterClassVectorMap;

QMap <QString,QString> objectpacket::ClassValueToString =
{
    {"00","WAR"},{"01","RAN"},{"02","PAL"},{"03","SK"},
    {"04","MNK"},{"05","BRD"},{"06","RGE"},{"07","DRD"},
    {"08","SHA"},{"09","CL"},{"0A","MAG"},{"0B","NEC"},
    {"0C","ENC"},{"0D","WIZ"},{"0E","ALC"}
};

QMap <QString,QString> objectpacket::RaceValueToString =
{
    {"00","HUM"},{"01","Elf"},{"02","DElf"},
    {"03","GNO"},{"04","DWF"},{"05","TRL"},
    {"06","BAR"},{"07","HLF"},{"08","ERU"},
    {"09","OGR"}
};


int Combat::ExtraDamage;


QVector<QString> Coaching::CoachmanIDsVector;

int Quests::PacketID_Check = 0;
QMap<QString,QMap<QString,QMap<int,QVector<QString>>>> Quests::QGID_OP_Flag_Popup_9sVectorMaps;
QMap<QString,QMap<QString,QMap<int,QVector<QString>>>> Quests::QGID_OP_Flag_Popup_36sVectorMaps;
QMap<QString,QMap<QString,QMap<int,QVector<QString>>>> Quests::QGID_OP_Flag_CheckVectorMaps;
QMap<QString,QMap<int,QString>> Quests::QuestGiversID_Flag_MenusID_Map;
QMap<QString,QVector<QString>> Quests::SpiritmastersID_MenuMap;
int Quests::SpiritMastersPacketID = 0;
QMap<QString,QMap<int,QString>> Quests::SpiritMastersID_PacketNumber_LocationMaps;
QMap<QString,QMap<int,QString>> Quests::LocationFlag_Menu_PopUpIDsMap;
QMap<QString,QVector<QString>> Quests::NPCsID_QuestIDsVectorMap;
QMap<QString,QMap<QVector<int>,QString>> Quests::LocationQuestID_QuestVectorsMap;
QMap<QString,QMap<QString,QVector<QString>>> Quests::QuestGiversID_Level_LocationClassRaceMap;
QMap<QString,QVector<QString>> Quests::PopUpID_PopUpResltsVectorMap;
int Quests::PacketID_Quests;
QString Quests::QuestsID;
QVector<QString> Quests::MenuTextCheckVector;
int Quests::FakeFlag;
QMap<QString,QVector<QString>> Quests::QuestsIDs_ResultsVectorMap;
QMap<QString,QMap<QString,QString>> Quests::PlayersID_QuestLogSlot_QuestID_MapOfMaps;
QMap<int,QVector<QString>> Quests::QuestValuesVectorMap;
QMap <QString,int> Quests::QuestGiversNames_IDS_Map;
QMap<QString,QString> Quests::QuestGiversID_NameMap;
QMap<QString,QString> Quests::DescriptionTextID_Text;
QString Quests::Decription_ID = "";
int Quests::PacketID_Decription = 0;
QMap<QString,QString> Quests::NPCsName_ZoneMap;
QMap<QString,QString> Quests::MenuID_MenuText_Map;
QMap<QString,QString> Quests::OptionID_OptionText_Map;
QString Quests::Logs_ID = "";
int Quests::PacketID_Logs = 0;
QMap<QString,QString> Quests::LogTextID_Text;
QString Quests::Text_ID = "";
QString Quests::Menu_ID = "";
QString Quests::Menu_Option_ID = "";
QString Quests::Popup_ID = "";
int Quests::PacketID_Text = 0;
int Quests::PacketID_Menu = 0;
int Quests::PacketID_Menu_Option = 0;
int Quests::PacketID_Popup = 0;
int Quests::PacketID_FlagMenuPopUpIDs = 0;
int Quests::Quest_Givers_PacketID = 0;
int Quests::PRE_REQS_PacketID = 0;
bool Quests::Quest = false;
QMap<QString,QString> Quests::Quests::PopUpID_PopUpText_Map;
QMap<QString,QVector<QString>> Quests::Quests::InGame_MenuesVectorMap;
QMap<QString,QMap<QString,QVector<QString>>> Quests::Quests::MenuID_OptionsIDs_ResultsVectorMap;

QMap<QByteArray,QVector <QByteArray>> Quests::NPC_Quests_IDs =
{
    {"00000000",{"01000000"}},
    {"FAF30400",{"02000000","03000000","04000000"}}
};

QMap<QString,bool> FacingDirection::FollowMap;



int Combat::PrimaryDMG = 0;
int Combat::SecondaryDMG = 0;
int Combat::Randomizer1 = 0;

////////////////////////////Auto Spawn Objectpacket///////////////////////////////////







QMap<QString,float> objectpacket::Temp_Distance_ID_Map;

///////////////////////////////Auto Spawn Objectpacket////////////////////////////////

///////////////////////////Generator//////////////

////////////////////////////////////////////////////


QMap<QString,QString> Opcodes::ItemsID_ItemsName_Map;
QString Opcodes::Old_ID = "";
QString Opcodes::RespawnID = "";
QString Opcodes::NewspawnID = "";
QMap<QString,QVector<QVector<QString>>> Opcodes::ID_VectorOfCoachesVectorMap;
QMap<QString,QVector<QByteArray>> Opcodes::CharID_CLientID_IP_PORT_Map;
QMap <QString,QString> Opcodes::ID_PacketNumber_Map;
QVector <QString> Opcodes::FactionIDVector;
QVector <int> Opcodes::FactionValueVector;
QVector <QString> Opcodes::StartingIDXZYF;
QVector <QString> Opcodes::EndingIDXZYF;
QMap <QString,QString> Opcodes::Name_PacketNumber_Map;
QMap <int,QString> Opcodes::NPC_Map;
QVector<QString> Opcodes::PlayerNames;
QString Opcodes::OpcodeOut;
QString Opcodes::OpcodeMessageOut;
QString Opcodes::GroupID = "9D030C00";
QMap<QString,QVector<QByteArray>> Opcodes::CharID_ServerMessageNumbers;
QMap <QString,QString> Opcodes::ObjectIDsMap;
int Opcodes::SessionsValue = 220761;
QString Opcodes::RandomName = "Chicken";
QMap<QString,QVector<QString>> Opcodes::Name_PacketStatusVectorMap;
QVector<QString> Opcodes::MasterVector;
QMap<QString,QVector<QString>> Opcodes::RaceGen_NameVectorMap;
QString Opcodes::m_FileName;
QMap<QString,QString> Opcodes::UserName_PasswordMap;
QMap<QString,int> Opcodes::UserName_PacketID;
QMap<QString,QVector<QString>> Opcodes::UserName_CharSlots;
QVector <QString> Opcodes::ServerFieldsVector;
int Opcodes::MaintoonTableSize = 0;
QByteArray Opcodes::MaintoonsCharID = "";
QMap<QString,QVector<QByteArray>> Opcodes::UserName_CharCountVectorMap;
QMap<QString,QVector<QString>> Opcodes::UserName_CharIDs;



//1
QByteArray Opcodes::Permafrost_tp = "f60700D1E5974505817642AE819745B0184640000000000000000000000000";
QByteArray Opcodes::Zentars_Keep_tp = "f607007E7EAB4505815842799EC945346E2840000000000000000000000000";
QByteArray Opcodes::Bogman_Village_tp = "f607001570A345D20558421319104651FECC3F000000000000000000000000";
QByteArray Opcodes::Mariel_Village_tp = "f6070000409C4507C1764200E02B46B338533F000000000000000000000000";
QByteArray Opcodes::Wyndhaven_tp = "f60700C63A934505A16D4212824E46F07B3C40000000000000000000000000";
QByteArray Opcodes::Whale_Hill_tp = "f60700F661924502092E42B00566468306BD3F000000000000000000000000";
QByteArray Opcodes::Qeynos_tp = "f60700E456964505A166427BD58546BBB80F3F000000000000000000000000";
QByteArray Opcodes::Qeynos_Prison_tp = "f607006375A44505A16642172995466293F03F000000000000000000000000";
QByteArray Opcodes::Highbourne_tp = "f60700EF959745058158429595A7461649AEBC000000000000000000000000";

//2
QByteArray Opcodes::Snowblind_Plains_tp = "f60700CF96DA4565B503431BD19B454A3D99BF000000000000000000000000";
QByteArray Opcodes::Unkempt_North_tp = "f60700AA6ADF458DF3AA42C49CDD4556DA1840000000000000000000000000";
QByteArray Opcodes::Unkempt_Glade_tp = "f60700B529DE4505C12C423EC90F467A20E4BE000000000000000000000000";
QByteArray Opcodes::Twisted_Tower_tp = "f60700A37CDA4531955B4244AC2946818745C0000000000000000000000000";
QByteArray Opcodes::Jethros_Cast_tp = "f607004E36DC45030BD6422F7D4A46FBDBA63D000000000000000000000000";
QByteArray Opcodes::Crethley_Manor_tp = "f607000D69D94505A16642F4256A466E250CC0000000000000000000000000";
QByteArray Opcodes::Hagley1_tp = "f60700EFEFDD4525C3134395AA854648BB46C0000000000000000000000000";
QByteArray Opcodes::Hagley2_tp = "f607009A23C44505A166426E208146313E543C000000000000000000000000";
QByteArray Opcodes::Druids_Watch_tp = "f607004369DD451998E2423DB293462E52FF3D000000000000000000000000";
QByteArray Opcodes::Stoneclaw_tp = "f60700CFBCDC45C0A4A1428A4EA34650A62D40000000000000000000000000";

//3
QByteArray Opcodes::Anu_Village_tp = "f607002A830946DC6856425834A645CAD3C33F000000000000000000000000";
QByteArray Opcodes::North_Wilderlands_tp = "f6070000A00C460561514200C0DA4500000000000000000000000000000000";
QByteArray Opcodes::Castle_Lightwolf_tp = "f6070010F71046058158423D66CB45205E46C0000000000000000000000000";
QByteArray Opcodes::Salisearaneen_tp = "f60700773C0D467CDDDE42119E0C4695640EC0000000000000000000000000";
QByteArray Opcodes::Murnf_tp = "f60700D6B60B46E36E5842D1572446133516C0000000000000000000000000";
QByteArray Opcodes::Surefall_Glade_tp = "f607008FCE0E4605A16642D03847466CCD34C0000000000000000000000000";
QByteArray Opcodes::Fog_Marsh_tp = "f6070000A00C460521654200606A4600000000000000000000000000000000";
QByteArray Opcodes::Honjour_tp = "f60700F839194605A16642EB456A46684C33C0000000000000000000000000";
QByteArray Opcodes::Bear_Cave_tp = "f6070060AA114605815842DC78854625FABC3F000000000000000000000000";
QByteArray Opcodes::Spider_Mine_tp = "f6070000A00C46058158420070944600000000000000000000000000000000";

//4
QByteArray Opcodes::Aviak_Village_tp = "f6070000A00C468A9096420010A44600000000000000000000000000000000";
QByteArray Opcodes::Frosteye_Vally_tp = "f60700460B2E4605815842A7DC994531170DC0000000000000000000000000";
QByteArray Opcodes::Guardian_Forest_tp = "f6070000E02B460581584200C0DA4500000000000000000000000000000000";
QByteArray Opcodes::Gramash_Ruins_tp = "f607008F02294605212E42D5E20B46D2423F40000000000000000000000000";
QByteArray Opcodes::Spirit_Talkers_Wood_tp = "f60700018D274607A96E42BCAB29466AE7BB3F000000000000000000000000";
QByteArray Opcodes::Spirit_Talkers_Wood2_tp = "f60700C86635461725BC429528344614D93A3F000000000000000000000000";
QByteArray Opcodes::Wymondham_tp = "f60700E622224605A166422F3543465103E5BF000000000000000000000000";
QByteArray Opcodes::Al_Karad_Ruins_tp = "f60700BB3B2946496F6B422957694698AA0FC0000000000000000000000000";
QByteArray Opcodes::Blakedown_tp = "f6070097E02C46E36E58420F82884685462940000000000000000000000000";
QByteArray Opcodes::Mayfly_Glade_tp = "f60700A3D1264695169A42CBF596465D344840000000000000000000000000";
QByteArray Opcodes::Urglunts_Wall_tp = "f60700BE1F3246AC58B942B5AAA546EFEA563D000000000000000000000000";

// 5
QByteArray Opcodes::Halas_tp = "f60700D8C84A467FE6544215319245322B67BF000000000000000000000000";
QByteArray Opcodes::Freezeblood_Village_tp = "f60700384C4D469B61A042581ADE45C3508E3F000000000000000000000000";
QByteArray Opcodes::Diren_Village_tp = "f6070069F54D46058158420E12034659CBE53F000000000000000000000000";
QByteArray Opcodes::Mt_Hatespike_tp = "f60700A51E5646058158422E6231464A6EBF3E000000000000000000000000";
QByteArray Opcodes::Blackburrow_tp = "f60700086A47460581584291C44C46125E16C0000000000000000000000000";
QByteArray Opcodes::Jareds_Blight_tp = "f60700B1604B4656F2EA4268116A46ACBED23F000000000000000000000000";
QByteArray Opcodes::Darvar_Manor_tp = "f60700004E5446058158420DA66646B367BE3F000000000000000000000000";
QByteArray Opcodes::Alseops_Wall_tp = "f60700F75D4546AE804542A673814674C41240000000000000000000000000";
QByteArray Opcodes::Forkwatch_tp = "f60700BDD643468300F042C68D9246CC0E93BC000000000000000000000000";
QByteArray Opcodes::South_Crossroads_tp = "f60700587B4B4602F364423D46A1462E24FE3F000000000000000000000000";
QByteArray Opcodes::Dark_Solace_tp = "f607001bde434669c4a742b683a5468d214840000000000000000000000000";
QByteArray Opcodes::Widows_Peak_tp = "f607004754524683F0C0423629B7461F1E4E3F000000000000000000000000";
QByteArray Opcodes::Kelinar_tp = "f60700F04B4446838088423F15C646CA4931C0000000000000000000000000";
QByteArray Opcodes::Gerntar_Mines_tp = "f60700D1A94B460501704213E5D646B1E6F0BF000000000000000000000000";
QByteArray Opcodes::Oggok_Gate_tp = "f60700934344460501704241E3E646ECCFF33E000000000000000000000000";
QByteArray Opcodes::Milas_Reef_tp = "f6070076AA4F4615DCCF42024FF0461CF5803F000000000000000000000000";
QByteArray Opcodes::Snowfist_tp = "f6070030166B46080C44425F239B4540B10D40000000000000000000000000";
QByteArray Opcodes::Goldfeather_Eyrie_tp = "f60700CD1E6C46426815432108E14599675A3F000000000000000000000000";
QByteArray Opcodes::Moradhim_tp = "f60700EDEB6F46C10C58425E061146C10D4040000000000000000000000000";
QByteArray Opcodes::Baga_Village_tp = "f60700545B6D46A0489342861E314687379B3F000000000000000000000000";
QByteArray Opcodes::Merry_by_Water_tp = "f6070023F3654605815842F4F34746DD55D4BF000000000000000000000000";
QByteArray Opcodes::Bandit_Hills_tp = "f607009CC3664605815842E52C634637191C40000000000000000000000000";
QByteArray Opcodes::Strags_Rest_tp = "f6070042537146E55A3B436FC8854694873D40000000000000000000000000";
QByteArray Opcodes::Salt_Mine_tp = "f60700AEDD654605815842EFCF94465630CBBF000000000000000000000000";
QByteArray Opcodes::Centaur_Vally_tp = "f60700A341654605815842C5C2A746EEE3B0BE000000000000000000000000";
QByteArray Opcodes::Brog_Fens_tp = "f60700249B63465CE759420A33B34677700540000000000000000000000000";
QByteArray Opcodes::Blackwater_tp = "f6070064F66C4605815842810EB1463108B2BF000000000000000000000000";
QByteArray Opcodes::Fort_Alliance_tp = "f60700D7AD764605815842C9D2C94654EBADBC000000000000000000000000";
QByteArray Opcodes::Elephant_Graveyard_tp = "f6070028DD7046058158423CF4D14669379CBD000000000000000000000000";
QByteArray Opcodes::Kerplunk_tp = "f60700E43F6746058158426173E446731DA73F000000000000000000000000";
QByteArray Opcodes::Cazic_Thule_tp = "f60700F265684642780143F338F246428F2D40000000000000000000000000";

//7
QByteArray Opcodes::Greyvaxs_Caves_tp = "f6070000D084468390964200409C4500000000000000000000000000000000";
QByteArray Opcodes::Snafitzers_House_tp = "f60700F9768546D95F7643CF59F1457EEFC93F000000000000000000000000";
QByteArray Opcodes::Shon_To_Monastery_tp = "f6070012F4864605A1664249850B46639E46C0000000000000000000000000";
QByteArray Opcodes::Misty_Thicket_tp = "f60700B3238646D5C07742F86D2F46FA1009BF000000000000000000000000";
QByteArray Opcodes::Runnyeye_tp = "f607006E158046C2AB9742793B3F46A45414C0000000000000000000000000";
QByteArray Opcodes::Highpass_tp = "f607004811834600d03b4391dd6f46C1F24F3F000000000000000000000000";
QByteArray Opcodes::Trails_End_tp = "f6070070428346058158425AD98A46F2FA22C0000000000000000000000000";
QByteArray Opcodes::Dshinns_Redoubt_tp = "f60700DBF78246E2FE6042BC2C934608C9373F000000000000000000000000";
QByteArray Opcodes::Wktaans_4th_Talon_tp = "f6070029158146AC10E042BA2BA9463D6384BF000000000000000000000000";
QByteArray Opcodes::Serpent_Hills_tp = "f6070086998A46F9D70043F6A9B346FCF3E3BF000000000000000000000000";
QByteArray Opcodes::Tak_Xiz_tp = "f60700863E854683B088422891C24626EA3B40000000000000000000000000";
QByteArray Opcodes::Tak_Xiz_South_tp = "f60700841A844605815842D89CD3469F146C3F000000000000000000000000";
QByteArray Opcodes::Lake_Noregard_tp = "f60700C1B285466A593F42DD84E246E24F86BE000000000000000000000000";
QByteArray Opcodes::Dinbak_tp = "f60700163D824605595242124CF446DA0F49C0000000000000000000000000";

//8
QByteArray Opcodes::Fayspire_Gate_tp = "f607008F13944696F29B4289819C45EBBDC4BF000000000000000000000000";
QByteArray Opcodes::Fayspires_tp = "f60700A216984605815842AA57DC45E1CB1040000000000000000000000000";
QByteArray Opcodes::Tethelin_tp = "f6070026DC8F4605815842526DEA4506F01740000000000000000000000000";
QByteArray Opcodes::Thedruk_tp = "f60700F4299546C0995342482F094612D10CBF000000000000000000000000";
QByteArray Opcodes::Rivervale_tp = "f60700C5498F4605815842522231460DAF0BBE000000000000000000000000";
QByteArray Opcodes::Moss_Mouth_Cavern_tp = "f60700BF959646E43CEF426F254846FCE745C0000000000000000000000000";
QByteArray Opcodes::Bastable_Village_tp = "f607001345944605815842AD3F6A46EB8B1340000000000000000000000000";
QByteArray Opcodes::Ferrans_Hope_tp = "f60700430098466ACF51425E3D8346877C91BF000000000000000000000000";
QByteArray Opcodes::Deathfist_Horde_tp = "f60700A1D3964607044943B61CA3464E073340000000000000000000000000";
QByteArray Opcodes::Chiktar_Hive_tp = "f60700450697465F6247438615B446F24466BF000000000000000000000000";
QByteArray Opcodes::Tak_Xiv_tp = "f6070015F69346058158421713C4461E170F40000000000000000000000000";
QByteArray Opcodes::Takish_Hiz_tp = "f60700F264984605E10B421E2CD646B0AC4840000000000000000000000000";
QByteArray Opcodes::Burial_Mounds_tp = "f607004A0E95469474BA42A0CEE44643C225BF000000000000000000000000";
QByteArray Opcodes::Stone_Watchers_tp = "f60700918199464E98F34245B9F54672534140000000000000000000000000";

//9
QByteArray Opcodes::NE_Mountain_Boundary_tp = "f607000010A4460B02C14100409C4500000000000000000000000000000000";
QByteArray Opcodes::Kara_Village_tp = "f60700BF8CA4464763734230B9D045C3048BBF000000000000000000000000";
QByteArray Opcodes::Castle_Felstar_tp = "f607000010A4460581584200A00C4600000000000000000000000000000000";
QByteArray Opcodes::Fort_Seriak_tp = "f607008825AA46058158423AE80246591B043E000000000000000000000000";
QByteArray Opcodes::North_Kithicor_tp = "f60700D012A646058158426EBB2646A09EB13F000000000000000000000000";
QByteArray Opcodes::Saerk_Towers_tp = "f6070079BCA54676B76D43226F524641A6C73E000000000000000000000000";
QByteArray Opcodes::Tomb_of_Kings_tp = "f60700EC97A44605212E42BA4D614627604640000000000000000000000000";
QByteArray Opcodes::Deathfist_Forge_tp = "f60700D5E0A3465D146642866F884606F2CCBF000000000000000000000000";
QByteArray Opcodes::Deathfist_Citadel_tp = "f60700FFE6A44683F0E7427FB294468D1F8CBF000000000000000000000000";
QByteArray Opcodes::Box_Canyons_tp = "f607009D4DA446210BCF40B637A446C7AAEFBF000000000000000000000000";
QByteArray Opcodes::Eternal_Desert_tp = "f60700D84CA44620D08A434251B346DB0FC9BF000000000000000000000000";
QByteArray Opcodes::Oasis_tp = "f6070064EBA4460581584206ABC446DB0FC93F000000000000000000000000";
QByteArray Opcodes::Sea_of_Lions_tp = "f607008492A4466FC6C7410ECBD2467F265B3F000000000000000000000000";
QByteArray Opcodes::Ant_Colonies_tp = "f607009009A8461F0D2043A4B7DE4649854140000000000000000000000000";
QByteArray Opcodes::Brokenskull_Rock_tp = "f6070052AFA346F8048942E938F7464DB1923C000000000000000000000000";

//10
QByteArray Opcodes::Klick_Anon_tp = "f60700C134B74605815842F964CA45EE0C3F3F000000000000000000000000";
QByteArray Opcodes::Collonridge_Cemetary_tp = "f60700BEB5AF46058158424BA80946CD51C03F000000000000000000000000";
QByteArray Opcodes::The_Green_Rift_tp = "f60700091EB04619043C41EFFC2846798825C0000000000000000000000000";
QByteArray Opcodes::Mu_Lins_Reach_tp = "f60700FB93B34683008C42D4964A462CCA1540000000000000000000000000";
QByteArray Opcodes::Temple_of_Light_tp = "f60700EE36B546F3007B420B9E70469D18843D000000000000000000000000";
QByteArray Opcodes::Northwestern_Ro_tp = "f6070046B3B346F4339142AEFC834663EBB3BE000000000000000000000000";
QByteArray Opcodes::Muniels_Tea_Garden_tp = "f607005F7FB64605815842D767994657A8A93F000000000000000000000000";
QByteArray Opcodes::Al_Farak_Ruins_tp = "f60700AE6DB64605AB62425DBFA2464C660AC0000000000000000000000000";
QByteArray Opcodes::Sycamore_Joys_Rest_tp = "f60700B9CEB446545E87421DA4B146823104BF000000000000000000000000";
QByteArray Opcodes::Great_Waste_tp = "f60700431BB046B4562443B782C246D5E047C0000000000000000000000000";
QByteArray Opcodes::Silthtar_Hive_tp = "f607004E8FB3464F26EF421175D346B1881BBF000000000000000000000000";
QByteArray Opcodes::Sslathis_tp = "f607000669B146BE0CB44280FBE4460B834840000000000000000000000000";
QByteArray Opcodes::Bashers_Enclave_tp = "f607009C9FB64605815842D54BEF4671253CC0000000000000000000000000";

//11
QByteArray Opcodes::Isle_of_Dread_tp = "f607006EFDBF460048524242C96245674A45C0000000000000000000000000";
QByteArray Opcodes::Rogue_Clockworks_tp = "f60700CFDBC246058158426C28E3453100C5BD000000000000000000000000";
QByteArray Opcodes::Neriak_tp = "f6070056B9C2466F77EB418E3113464942B13D000000000000000000000000";
QByteArray Opcodes::Bobble_by_Water_tp = "f60700589DC04605815842B33439463BAE65BF000000000000000000000000";
QByteArray Opcodes::Hodstock_and_Temby_tp = "f60700C776C846D14554423C5F504690A1CABF000000000000000000000000";
QByteArray Opcodes::Freeport_tp = "f607001072c546008058422aad75462fe04840000000000000000000000000";
QByteArray Opcodes::Northern_Ro_tp = "f60700967CC44605815842CBAE8246824ADCBF000000000000000000000000";
QByteArray Opcodes::Elemental_Towers_tp = "f6070065C6C14682ADA0425FE6C3468DA2013F000000000000000000000000";
QByteArray Opcodes::Hazinak_Dock_tp = "f607007061C44605815842BC27D446382FACBF000000000000000000000000";
QByteArray Opcodes::Hazinak_Temple_tp = "f607006F65C246310C95424C9CD746EA7330BF000000000000000000000000";
QByteArray Opcodes::Guk_tp = "f60700951BC146058158421E0DE646A80BC43F000000000000000000000000";
QByteArray Opcodes::Grobb_tp = "f6070027FCC44605815842F37AF3462248413F000000000000000000000000";

//Rathe
QByteArray Opcodes::Oggok_tp = "f6070130D70B46F1805842C9A9E0459F57BA3F000000000000000000000000";

//Odus
QByteArray Opcodes::Phantoms_tp = "f60702B0CBDC4505A16642BCBE5A45DD934740000000000000000000000000";

//Lavastorm Mountains
QByteArray Opcodes::Waysender_tp = "f60703B610F7458390964229D5B84550241AC0000000000000000000000000";

//Plane of Sky
QByteArray Opcodes::Pos_Main_Isle = "f607048EBF9645426C7A43C700AC45662D4840000000000000000000000000";
QByteArray Opcodes::Isle_of_Wonder = "f607048B409C45BAC4A64303409C45D2B049C0000000000000000000000000";
QByteArray Opcodes::Sun_1_in = "f60704C719AF4542006B432EDEAB45A45B4740000000000000000000000000";
QByteArray Opcodes::Sun_1_village = "f60704C170B44520008C431DD29F45AC4CCABF000000000000000000000000";
QByteArray Opcodes::Sun_1_out = "f6070414CCB045CD0C8C43B89A9F45AC4CCABF000000000000000000000000";
QByteArray Opcodes::Sun_2_in = "f60704EA5EB045A46BA343AD4699458A1C48C0000000000000000000000000";
QByteArray Opcodes::Sun_2_ramp = "f60704CD06B845A410A243E1849345AC4CCABF000000000000000000000000";
QByteArray Opcodes::Sun_2_out = "f607044E28A84586498D43A6A09445F436C6BF000000000000000000000000";
QByteArray Opcodes::Sun_3_in = "f60704433BB3452004CA433A7A83451D02CABF000000000000000000000000";
QByteArray Opcodes::Sun_3_out = "f607048597AA45CD8CBE43C3698445AC4CCABF000000000000000000000000";
QByteArray Opcodes::Sun_4_in = "f60704150EA3452070D74306C68345DB0FC9BF000000000000000000000000";
QByteArray Opcodes::Sun_4_out = "f607049AE99D45D7E3D7437B988445DB0FC9BF000000000000000000000000";
QByteArray Opcodes::Rain_1_in = "f607047D09844542206543E408AC4508EE3F3D000000000000000000000000";
QByteArray Opcodes::Rain_1_out = "f607048FF483450000694333A3B24508EE3F3D000000000000000000000000";
QByteArray Opcodes::Rain_2_in = "f607046F8381454200344379C0A3454AFF48C0000000000000000000000000";
QByteArray Opcodes::Rain_2_out = "f60704D7F5814566E63443B808A0454AFF48C0000000000000000000000000";
QByteArray Opcodes::Rain_3_in = "f60704AD608945D714DE42E8778E45E54F893F000000000000000000000000";
QByteArray Opcodes::Rain_3_out = "f60704E1EC90450A57EF4200729145E54F893F000000000000000000000000";
QByteArray Opcodes::Rain_4_in = "f60704BB3F8445E934A142B6108645962041C0000000000000000000000000";
QByteArray Opcodes::Rain_4_out = "f607045C3F8345CDCCA142CD968145962041C0000000000000000000000000";
QByteArray Opcodes::Rain_5_in = "f60704375696450B02F041C3A686451075E1BF000000000000000000000000";
QByteArray Opcodes::Rain_5_out = "f607043D3E8F4514AEF1419A0386451075E1BF000000000000000000000000";
QByteArray Opcodes::Night_1_in = "f60704EE0382450F1819447F79BD45DFC446BB000000000000000000000000";
QByteArray Opcodes::Night_1_out = "f60704660C8245EC5119440A1FC845DFC446BB000000000000000000000000";
QByteArray Opcodes::Night_2_in = "f607045DC88A452058E0435994C24500000000000000000000000000000000";
QByteArray Opcodes::Night_2_out = "f60704AECD8A4585CBE043E12AC94500000000000000000000000000000000";
QByteArray Opcodes::Night_3_in = "f60704B0CF84450F640E447CC4CC455EECADBA000000000000000000000000";
QByteArray Opcodes::Night_3_out = "f60704AECD844514DE27447BBCD4455EECADBA000000000000000000000000";
QByteArray Opcodes::Night_4_in = "f6070441038145208CB0433FF4EC450D2D613F000000000000000000000000";
QByteArray Opcodes::Night_4_out = "f60704481587450000B143524AF2450D2D613F000000000000000000000000";
QByteArray Opcodes::Night_5_in = "f60704D047814542485043343BDA45BF25D73F000000000000000000000000";
QByteArray Opcodes::Night_5_out = "f60704C383894566A6024352F6DE45BF25D73F000000000000000000000000";
QByteArray Opcodes::Snow_1_in = "f60704F093B1450F6E0A44469CC8458A1C4840000000000000000000000000";
QByteArray Opcodes::Snow_1_center = "f60704AEC7B145CD0CF44348DFCC458A1C4840000000000000000000000000";
QByteArray Opcodes::Snow_1_bottom = "f60704ECCBA645297CDB4329A0BF458A1C4840000000000000000000000000";
QByteArray Opcodes::Snow_2_in = "f6070487D3B34531686D4354B2D4458A1C4840000000000000000000000000";
QByteArray Opcodes::Snow_2_out = "f60704B842B4455C4F6E43AE6BD1458A1C4840000000000000000000000000";
QByteArray Opcodes::Snow_3_in = "f60704A4B1B64542A0304344E2DD453EB698BC000000000000000000000000";
QByteArray Opcodes::Snow_3_out = "f607045C97B845AE873143148CEC453EB698BC000000000000000000000000";
QByteArray Opcodes::Snow_4_in = "f607044A43AA458300BE4234F9E545AA5B9D3B000000000000000000000000";
QByteArray Opcodes::Snow_4_pad = "f607043D3EAC45D723D54248C9ED45AA5B9D3B000000000000000000000000";
QByteArray Opcodes::Snow_4_out = "f6070452B0AC453333C042CDD8F045AA5B9D3B000000000000000000000000";
QByteArray Opcodes::Deso_1_in = "f60704D6B296450F000E445D89BF45FF7F21BB000000000000000000000000";
QByteArray Opcodes::Deso_1_out = "f60704EC7594459A390E44C38FC845FF7F21BB000000000000000000000000";
QByteArray Opcodes::Deso_2_in = "f60704E94C8F45F6181A441389CC450DAE3EC0000000000000000000000000";
QByteArray Opcodes::Deso_2_village = "f60704D7959745A4F019447B28D2450DAE3EC0000000000000000000000000";
QByteArray Opcodes::Deso_2_out = "f607045CDDA0459A291A4466DECE450DAE3EC0000000000000000000000000";
QByteArray Opcodes::Deso_3_in = "f607045825A1450F5631448800D845BE9A043F000000000000000000000000";
QByteArray Opcodes::Deso_3_out = "f607045CD59F45EC6132444837E045BE9A043F000000000000000000000000";
QByteArray Opcodes::Deso_4_in = "f6070433999B458F124944A4F8E645BE9A043F000000000000000000000000";
QByteArray Opcodes::Deso_4_out = "f6070400F29345801D5D44B03FF045BE9A043F000000000000000000000000";

//Secrets
QByteArray Opcodes::Last_Home_tp = "f607051A249145E7A03CC111AC6E4508F78A40000000000000000000000000";
QByteArray Opcodes::Last_Home_2 = "f60705B80F9045E7FB3EC1A6EE61454D39CCBF000000000000000000000000";
QByteArray Opcodes::Pod_1 = "f60705CA3B9E452081C5C1718C724599004840000000000000000000000000";
QByteArray Opcodes::Pod_2 = "f6070534B5B145F2FD9FC106F86C45EFEF1D40000000000000000000000000";
QByteArray Opcodes::Pod_3 = "f607052B318245EBA07EC165B210458FCC48C0000000000000000000000000";
QByteArray Opcodes::Pod_4 = "f60705EC43984572909BC1AEBD3A45AD0839C0000000000000000000000000";

//ZP
QByteArray Opcodes::Zp_1 = "f60705C046984542A81343943FDC45B04DCBBF000000000000000000000000";
QByteArray Opcodes::Zp_2 = "f607058406B24542D85A43A660C44560A648C0000000000000000000000000";
QByteArray Opcodes::Zp_3 = "f60705F1B5A645206CA8439CAAEC45A0ED4940000000000000000000000000";
QByteArray Opcodes::Zp_4 = "f6070507828B4542207E437E45C2451A2D49C0000000000000000000000000";
QByteArray Opcodes::Zp_5 = "f60705A79F8B45438837436326CB4546784A40000000000000000000000000";
QByteArray Opcodes::Zp_6 = "f60705C8238845438837436B70D2452F374740000000000000000000000000";
QByteArray Opcodes::Zp_7 = "f60705219E884542207E43995DDC458EF14740000000000000000000000000";

QMap <int,QByteArray> Opcodes::coach_destination_map = {{2,Opcodes::Blackwater_tp},{4,Opcodes::Bobble_by_Water_tp},
                                                        {6,Opcodes::Castle_Lightwolf_tp},{8,Opcodes::Dark_Solace_tp},
                                                        {10,Opcodes::Darvar_Manor_tp},{12,Opcodes::Fayspires_tp},
                                                        {14,Opcodes::Honjour_tp},{16,Opcodes::Forkwatch_tp},
                                                        {18,Opcodes::Fort_Seriak_tp},{20,Opcodes::Freeport_tp},
                                                        {22,Opcodes::Gerntar_Mines_tp},{24,Opcodes::Grobb_tp},
                                                        {26,Opcodes::Halas_tp},{28,Opcodes::Hazinak_Dock_tp},
                                                        {30,Opcodes::Hazinak_Temple_tp},{32,Opcodes::Highbourne_tp},
                                                        {34,Opcodes::Highpass_tp},{36,Opcodes::Kerplunk_tp},
                                                        {38,Opcodes::Klick_Anon_tp},{40,Opcodes::Moradhim_tp},
                                                        {42,Opcodes::Mt_Hatespike_tp},{44,Opcodes::Muniels_Tea_Garden_tp},
                                                        {46,Opcodes::Murnf_tp},{48,Opcodes::Neriak_tp},
                                                        {50,Opcodes::Oasis_tp},{52,Opcodes::Oggok_tp},
                                                        {54,Opcodes::Qeynos_tp},{56,Opcodes::Rivervale_tp},
                                                        {58,Opcodes::Surefall_Glade_tp},{60,Opcodes::Tethelin_tp},
                                                        {62,Opcodes::Wyndhaven_tp},{64,Opcodes::Zentars_Keep_tp}};


Loot::Loot(QObject *parent) : QObject(parent)
{

}


QString Loot::Loot_packet(QString Mob_id)
{
    Loot_list.clear(); //clear the list
    int Mobid_int = 0x00; //change clicked on id to longlong


    QMap <QString,int> Mob_Map =
    {
        //pickclaw brawler
        {"71E50000",0x01},{"74E50000",0x01},{"75E50000",0x01},{"77E50000",0x01},{"78E50000",0x01},
        {"79E50000",0x01},{"7AE50000",0x01},{"83E50000",0x01},{"84E50000",0x01},{"AA071000",0x01},
        {"AB071000",0x01},{"AC071000",0x01},{"AF071000",0x01},{"B0071000",0x01},{"B1071000",0x01},

        //pickclaw shawman
        {"73E50000",0x02},{"76E50000",0x02},{"7BE50000",0x02},{"7DE50000",0x02},{"7EE50000",0x02},
        {"81E50000",0x02},{"85E50000",0x02},{"86E50000",0x02},{"AE071000",0x02},{"B2071000",0x02},
        {"B3071000",0x02}
    };



    if(Mob_Map.contains(Mob_id))
    {
        Mobid_int = Mob_Map.value(Mob_id);
    }

    qDebug() << "Mobid_int" << Mobid_int;
    qDebug() << "Mob_id" << Mob_id;

    switch (Mobid_int)
    {

    case  0x01: //
        Loot_list =
        {
            {
                "9688808000","9888808000"
            }
        };
        break;

    case 0x02: //
        Loot_list =
        {
            {
                "9688808000","9A88808000"
            }
        };
        break;

    case 0xFF: //
        Loot_list =
        {
            {
                "0000000000","0000000000"
            }
        };
        break;

    default:
        QString nomatch = "NULL";
        return nomatch.toUtf8();
    }

    for (int ce = 0; ce < Loot_list.size(); ce++) //main vector from any merchant
    {
        QString itemid1 = Loot_list.at(ce);

        if(merchants::Items_Map.contains(itemid1.toUtf8()))
        {
            QString mer_result1 = merchants::Items_Map.value(itemid1.toUtf8()); //get item line
            QString inventory_rev = QString("%1").arg(ce,8,16,QLatin1Char('0')); //change ce to 8 char line
            qlonglong inv_reverse = inventory_rev.toLongLong(nullptr,16); //change to longlong
            quint32 revnum1 = ((inv_reverse & 0xFF) << 24) | ((inv_reverse & 0xFF00) << 8) | ((inv_reverse >> 8) & 0xFF00) | (inv_reverse >> 24); //reverse it
            QString inventory_num1 = QString("%1").arg(revnum1,8,16,QLatin1Char('0')); //change to string
            change_inventory = inventory_num1.toUtf8(); //change to array
            //qDebug() << "change" << change_inventory;
            int inv_pos1 = mer_result1.indexOf("XXXXXXXX"); //find index of line
            mer_result1.replace(inv_pos1,8,change_inventory); //write new inventory number

            total_packet.append(mer_result1);
        }
    }

    int num_items = Loot_list.size(); //get number of times looped
    QString new_num_items = QString("%1").arg(num_items,2,16,QLatin1Char('0'));
    total_packet.insert(0,new_num_items.toUtf8()); //replace new item count

    int doubled_num_items = num_items * 0x2;
    QString new_doubled_num_items = QString("%1").arg(doubled_num_items,2,16,QLatin1Char('0'));
    total_packet.insert(2,new_doubled_num_items.toUtf8()); //replace doubled count

    return total_packet; //return the filled map

}

bool Loot::AddLoot(QByteArray ID_IP_PORT, QString ItemsID, QString Quantity)
{
    packetparsing::packetvars IN_AddLoot;
    IN_AddLoot = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);



    qDebug() << "IN_AddLoot.LootWindowsID" << IN_AddLoot.LootWindowsID;
    qDebug() << "IN_AddLoot.ItemsID" << ItemsID;

    QVector <QString> ItemsFields = merchants::MasterItems_Vector_Map.value(ItemsID);

    QString ItemsMaxSize = ItemsFields.at(23);
    QString CostPerItem = ItemsFields.at(10);
    QString ArmorColor = ItemsFields.at(0);
    QString WeaponGraphic = ItemsFields.at(1);
    QString ArmorGraphic = ItemsFields.at(2);
    QString Charges = ItemsFields.at(5);
    QString EQuipmentsType = ItemsFields.at(14);
    QString DoesItHaveCharges = "";
    QString ADD_ItemTotal = "";
    QString ConvertedValue = "";
    QString FBsizeOpcode = "";

    bool NotStackable;
    bool NewItem;
    bool ItemsFull;
    bool sendornot = true;

    if(Charges != "00")
    {
        DoesItHaveCharges = "YES";
    }
    else
    {
        DoesItHaveCharges = "NO";
    }

    if(ItemsMaxSize == "02")
    {
        NotStackable = true;
    }
    else
    {
        NotStackable = false;
    }

    ADD_OR_SUB ConvertBack;
    QString BackToDec = "";
    int QuantityLocation = 0;
    int InventoryID = 0;
    QString Inventoryidxxxxxxxx = "";
    QString CurrentInventoryQuantity = "";
    Convert Send;
    ADD_OR_SUB SendToText;
    QString FBText = "";
    QString TextFull = "";

    if(IN_AddLoot.InventorySlots_ItemIDs.contains(ItemsID))
    {
        if(NotStackable == false) // if it is a stack and it is in the inventory already
        {
            NewItem = false;
            QuantityLocation = IN_AddLoot.InventorySlots_ItemIDs.indexOf(ItemsID);
            Inventoryidxxxxxxxx = IN_AddLoot.Inventory_Vector.at(QuantityLocation);
            CurrentInventoryQuantity = IN_AddLoot.InventoryItem_Quantity.at(QuantityLocation);
        }
        else
        {
            CurrentInventoryQuantity = "00";
            NewItem = true;
        }
    }
    else
    {
        CurrentInventoryQuantity = "00";
        NewItem = true;
    }

    qDebug() << "NotStackable" << NotStackable;
    qDebug() << "CurrentInventoryQuantity" << CurrentInventoryQuantity;
    qDebug() << "NewItem" << NewItem;

    QString Item_Quantity1 = Quantity.mid(0,2);
    QString Item_Quantity2 = Quantity.mid(2,2);
    QString Item_Quantity3 = Quantity.mid(4,2);
    int Item_Quantity4 = Item_Quantity1.toInt(nullptr,16);
    int Item_Quantity5 = Item_Quantity2.toInt(nullptr,16);

    QString Item_Quantity6 = "";

    if(Item_Quantity4 >= 0x80)
    {
        if(Item_Quantity5 >= 0x80)
        {
            Item_Quantity6 = Item_Quantity1 + Item_Quantity2 + Item_Quantity3;
        }
        else
        {
            Item_Quantity6 = Item_Quantity1 + Item_Quantity2;
        }
    }
    else
    {
        Item_Quantity6 = Item_Quantity1;
    }

    BackToDec = ConvertBack.BackToDec(Item_Quantity6);
    int ADDQuantity = BackToDec.toInt(nullptr,10);
    BackToDec = ConvertBack.BackToDec(CurrentInventoryQuantity);
    int InvQuantity = BackToDec.toInt(nullptr,10);
    BackToDec = ConvertBack.BackToDec(ItemsMaxSize);
    int ItemsMaxSize2 = BackToDec.toInt(nullptr,10);
    int total = InvQuantity + ADDQuantity;

    qDebug() << "ADDQuantity" << ADDQuantity;
    qDebug() << "InvQuantity" << InvQuantity;
    qDebug() << "total" << total;

    if(total <= ItemsMaxSize2 || NotStackable == true)//if the stack can fit or its not stackable
    {
        ItemsFull = false; // false = the item is not full

        ADD_ItemTotal = QString("%1").arg(ADDQuantity,2,10,QLatin1Char('0')).toUpper();
        ADD_ItemTotal = Send.ConvertTo(ADD_ItemTotal);

        QString ADD_Item2 = QString("%1").arg(total,2,10,QLatin1Char('0')).toUpper();

        qDebug() << "ADD_Item2A" << ADD_Item2;

        ConvertedValue = Send.ConvertTo(ADD_Item2);
    }

    if(total > ItemsMaxSize2)// if we try to ADD more than we can fit
    {
        int total2 = total - ItemsMaxSize2;// how much we cant fit
        int total3 = ADDQuantity - total2;// subtract what we cant fit

        if(total3 > 0)//if what we can fit is less then the max stack size
        {
            // if you can fit more than 0 // we can probably make it do this part only once
            ItemsFull = false;//then Item is not full

            ADD_ItemTotal = QString("%1").arg(total3,2,10,QLatin1Char('0')).toUpper();
            ADD_ItemTotal = Send.ConvertTo(ADD_ItemTotal);

            QString ADD_Item2 = QString("%1").arg(total3 + InvQuantity,2,10,QLatin1Char('0')).toUpper();

            qDebug() << "ADD_Item2B" << ADD_Item2;

            ConvertedValue = Send.ConvertTo(ADD_Item2);

        }
        else
        {
            ItemsFull = true;
            qDebug() << "ItemsFull4 =" << ItemsFull;
        }
    }

    qDebug() << "ItemsFull" << ItemsFull;

    qDebug() << "IN_AddLoot.InventoryMAX" << IN_AddLoot.InventoryMAX;

    if(
            (IN_AddLoot.InventoryMAX < 40 && ItemsFull == false) ||
            (IN_AddLoot.InventoryMAX == 40 && ItemsFull == false && NotStackable == false
             && NewItem == false))
    {
        QString ADD_Item = merchants::Items_Map.value(ItemsID);

        ADD_Item.replace(0,2,ADD_ItemTotal.toUtf8().toUpper());//change the number of items to what we are ADDing

        if(NewItem == true)
        {
            InventoryID = ADD_Item.indexOf("XXXXXXXX");
            increment ADD;
            QByteArray ADD2 = ADD.count(IN_AddLoot.InventoryIDCounter); //InventoryID increment
            ADD_Item.replace(InventoryID,8,IN_AddLoot.InventoryIDCounter + "0000");
            IN_AddLoot.InventoryIDCounter = ADD2.toUpper();

            IN_AddLoot.InventorySlots_ItemIDs.append(ItemsID);

            qDebug() << "IN_AddLoot.ItemsID" << ItemsID;

            qDebug() << "IN_AddLoot.InventorySlots_ItemsIDs" << IN_AddLoot.InventorySlots_ItemIDs;

            IN_AddLoot.Inventory_Vector.append(IN_AddLoot.InventoryIDCounter + "0000");

            IN_AddLoot.InventoryMAX++;

            IN_AddLoot.ItemIsEquippedVector.append("NO");
            IN_AddLoot.EQuipSots_InvSlots.append("XX");

            if(DoesItHaveCharges == "NO")
            {
                qDebug() << "IN_AddLoot.InventoryItem_QuantityA =" << IN_AddLoot.InventoryItem_Quantity;
                IN_AddLoot.InventoryItem_Quantity.append(ConvertedValue);
                qDebug() << "IN_AddLoot.InventoryItem_QuantityB =" << IN_AddLoot.InventoryItem_Quantity;
            }
            else
            {
                IN_AddLoot.InventoryItem_Quantity.append(Charges);
                qDebug() << "IN_AddLoot.InventoryItem_Quantity =" << IN_AddLoot.InventoryItem_Quantity;
            }

            if(EQuipmentsType == "01")
            {
                IN_AddLoot.InvItem_Type.append("Other");
            }
            else
            {
                IN_AddLoot.InvItem_Type.append("EQuipment");
            }

            packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_AddLoot);

            IN_AddLoot = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
        }

        if(NotStackable == false && NewItem == false)
        {
            InventoryID = ADD_Item.indexOf("XXXXXXXX");
            ADD_Item.replace(InventoryID,8,Inventoryidxxxxxxxx.toUtf8().toUpper());
            IN_AddLoot.InventoryItem_Quantity.replace(QuantityLocation,ConvertedValue);

            packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_AddLoot);

            IN_AddLoot = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
        }
    }
    else
    {
        if(NotStackable == false) // if you can afford it and it is a stack item check the next condition or go to the else with red text
        {
            if(NewItem == false) // if you can afford it and it is a stack item and it is not a new item send this red text or go to the else with the other red text
            {
                //this part is for invtory is full opcode
                IN_AddLoot.SendFBsVector.append("D700");
                IN_AddLoot.FBReplyOrNotVector.append("03");
                IN_AddLoot.FBPacketDescriptionVector.append("Fake_Opcode, ");
                //this part is for invtory is full opcode

                QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_AddLoot.CharSelectID);
                QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(0);

                TextFull = "Your inventory for that stack item is full.";
                FBText =  SendToText.TextSize(IN_AddLoot.ClientsID,IN_AddLoot.ClientsIP,IN_AddLoot.ClientsPort,TextFull,true,"CE00",Servers_Last_FB_MessageNumber);

                IN_AddLoot.SendFBsVector.append(FBText.toUtf8());
                IN_AddLoot.FBReplyOrNotVector.append("03");
                IN_AddLoot.FBPacketDescriptionVector.append("Inventory_Full, ");

                sendornot =  false;

                packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_AddLoot);

                IN_AddLoot = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

            }
            else
            {
                //this part is for invtory is full opcode
                IN_AddLoot.SendFBsVector.append("D700");
                //this part is for invtory is full opcode
                IN_AddLoot.FBReplyOrNotVector.append("03");
                IN_AddLoot.FBPacketDescriptionVector.append("Fake_Opcode, ");

                QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_AddLoot.CharSelectID);
                QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(0);

                TextFull = "Your inventory is full.";
                FBText =  SendToText.TextSize(IN_AddLoot.ClientsID,IN_AddLoot.ClientsIP,IN_AddLoot.ClientsPort,TextFull,true,"CE00",Servers_Last_FB_MessageNumber);

                IN_AddLoot.SendFBsVector.append(FBText.toUtf8());
                IN_AddLoot.FBReplyOrNotVector.append("03");
                IN_AddLoot.FBPacketDescriptionVector.append("Inventory_Full, ");

                sendornot =  false;

                packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_AddLoot);

                IN_AddLoot = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
            }
        }
        else
        {
            //this part is for invtory is full opcode
            IN_AddLoot.SendFBsVector.append("D700");
            IN_AddLoot.FBReplyOrNotVector.append("03");
            IN_AddLoot.FBPacketDescriptionVector.append("Fake_Opcode, ");
            //this part is for invtory is full opcode

            QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_AddLoot.CharSelectID);
            QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(0);

            TextFull = "Your inventory is full.";
            FBText =  SendToText.TextSize(IN_AddLoot.ClientsID,IN_AddLoot.ClientsIP,IN_AddLoot.ClientsPort,TextFull,true,"CE00",Servers_Last_FB_MessageNumber);

            IN_AddLoot.SendFBsVector.append(FBText.toUtf8());
            IN_AddLoot.FBReplyOrNotVector.append("03");
            IN_AddLoot.FBPacketDescriptionVector.append("Inventory_Full, ");

            sendornot =  false;

            packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_AddLoot);

            IN_AddLoot = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
        }
    }

    packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_AddLoot);

    qDebug() << "IN_AddLoot.InventorySlots_ItemIDs2" << IN_AddLoot.InventorySlots_ItemIDs;

    flipped sento;
    sento.SendOut(IN_AddLoot.ClientsID, IN_AddLoot.ClientsIP, IN_AddLoot.ClientsPort);

    IN_AddLoot = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

    qDebug() << "IN_AddLoot.InventorySlots_ItemIDs3" << IN_AddLoot.InventorySlots_ItemIDs;

    return sendornot;

}
