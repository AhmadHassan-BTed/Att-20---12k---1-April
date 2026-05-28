#include "combat.h"
#include "packetparsing.h"
#include "generator.h"
#include "objectpacket.h"
#include "opcodes.h"
#include "checkthezone.h"


Combat::Combat(QObject *parent) : QObject(parent)
{

}

void Combat::SendToMAXMeleeDMG(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QString AttackersID, QString AttackersLevel, QString DefendersID, QString DefendersLevel, QString AttackersSTR, QString AttackersDEX, QString AttackersOffMods, QString DefendersAGI, QString DefendersAC, QString DefendersDefMods)
{
    packetparsing::packetvars IN_Combat;
    IN_Combat = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    QVector <QString> typeWeaponVect;
    QVector <QString> DamageVector;

    QByteArray WeaponTypeSecondary = "";
    QByteArray WeaponTypePrimary = "";
    QByteArray WeaponType = "";

    QString SecondaryLevel = "";
    QString SecondaryDamage = "";
    QString PrimaryLevel = "";
    QString PrimaryDamage = "";
    QString Class = "";
    QString AttakersName = "";
    QString DefendersName = "";


    int Attacker_LVL = 0;
    int Attacker_STR = 0;
    int Attacker_DEX = 0;
    int Attacker_Offmods = 0;
    int Defender_LVL = 0;
    int Defender_AGI = 0;
    int Defender_AC = 0;
    int Defender_Defmods = 0;
    int WPN_LVL = 0;
    int WPN_DMG = 0;
    int SwingCount = 0;



    if(generator::Players_Map.contains(AttackersID.toUtf8()))
    {
//        WeaponTypeSecondary = IN_Combat.WeaponTypeSecondary;
//        WeaponTypePrimary = IN_Combat.WeaponTypePrimary;
//        SecondaryLevel = IN_Combat.SecondaryLevel;
//        SecondaryDamage = IN_Combat.SecondaryDamage;
//        PrimaryLevel = IN_Combat.PrimaryLevel;
//        PrimaryDamage = IN_Combat.PrimaryDamage;


        AttakersName = checkthezone::NPCsNames.value(IN_Combat.CharSelectID);

        WeaponTypeSecondary = "1H";
        WeaponTypePrimary = "1H";
        SecondaryLevel = AttackersLevel;
        SecondaryDamage = QString::number(AttackersLevel.toInt(nullptr,10) * 10);
        PrimaryLevel = AttackersLevel;
        PrimaryDamage = QString::number(AttackersLevel.toInt(nullptr,10) * 10);

        Attacker_LVL = AttackersLevel.toInt(nullptr,10);
        Attacker_STR = AttackersSTR.toInt(nullptr,10);
        Attacker_DEX = AttackersDEX.toInt(nullptr,10);
        Attacker_Offmods = AttackersOffMods.toInt(nullptr,10);

        Class = IN_Combat.Class;

        DefendersName = checkthezone::NPCsNames.value(DefendersID);

        Defender_LVL = DefendersLevel.toInt(nullptr,10);
        Defender_AGI = DefendersAGI.toInt(nullptr,10);
        Defender_AC = DefendersAC.toInt(nullptr,10);
        Defender_Defmods = DefendersDefMods.toInt(nullptr,10);

    }
    else
    {
        int Total = Attacker_STR + Attacker_DEX;
        int Average = Total / 2;

        AttakersName = checkthezone::NPCsNames.value(AttackersID);

        WeaponTypeSecondary = "Bare Hands";
        WeaponTypePrimary = "Bare Hands";
        SecondaryLevel = AttackersLevel;
        SecondaryDamage = QString::number(Average);
        PrimaryLevel = AttackersLevel;
        PrimaryDamage = QString::number(Average);
        Attacker_LVL = AttackersLevel.toInt(nullptr,10);
        Attacker_STR = AttackersSTR.toInt(nullptr,10);
        Attacker_DEX = AttackersDEX.toInt(nullptr,10);
        Attacker_Offmods = 0;


        Class = "00";

        DefendersName = checkthezone::NPCsNames.value(DefendersID);

        Defender_LVL = DefendersLevel.toInt(nullptr,10);
        Defender_AGI = DefendersAGI.toInt(nullptr,10);
        Defender_AC = DefendersAC.toInt(nullptr,10);
        Defender_Defmods = DefendersDefMods.toInt(nullptr,10);

    }

    QMap <QString,int> ClassSwingCountMap =
    {
        {"00",4},{"01",4},{"02",2},{"03",2},
        {"04",4},{"05",4},{"06",4},{"07",1},
        {"08",1},{"09",1},{"0A",1},{"0B",1},
        {"0C",1},{"0D",1},{"0E",1}
    };

    SwingCount = ClassSwingCountMap.value(Class);

    qDebug() << "";

    qDebug() << "AttakersName" << AttakersName;
    qDebug() << "AttackersID" << AttackersID;
    qDebug() << "AttackersClass" << Class;
    qDebug() << "AttackersLevel" << AttackersLevel;
    qDebug() << "Attacker_LVL" << Attacker_LVL;
    qDebug() << "Attacker_STR" << Attacker_STR;
    qDebug() << "Attacker_DEX" << Attacker_DEX;
    qDebug() << "Attacker_Offmods" << Attacker_Offmods;

    qDebug() << "";

    qDebug() << "DefendersName" << DefendersName;
    qDebug() << "DefendersID" << DefendersID;
    qDebug() << "Defender_LVL" << Defender_LVL;
    qDebug() << "Defender_AGI" << Defender_AGI;
    qDebug() << "Defender_AC" << Defender_AC;
    qDebug() << "Defender_Defmods" << Defender_Defmods;

    qDebug() << "";

    if(WeaponTypeSecondary == "Ranged")
    {
        SwingCount = 1;
    }

    if(WeaponTypePrimary == "2H")
    {
        if(SwingCount != 1)
        {
            SwingCount = 2;

            if(Attacker_LVL < 20)
            {
                SwingCount = 1;
            }
        }
    }

    //With 1 Weapon or 2
    if(WeaponTypePrimary == "1H" && WeaponTypeSecondary == "1H" && Attacker_LVL >= 20 && Attacker_LVL <= 34 && SwingCount == 4)
    {
        SwingCount = 2;
    }

    if(WeaponTypePrimary == "1H" && WeaponTypeSecondary == "Bare Hands" && Attacker_LVL >= 20 && Attacker_LVL <= 34 && SwingCount == 2)
    {
        SwingCount = 2;
    }

    if(WeaponTypePrimary == "1H" && WeaponTypeSecondary == "1H" && Attacker_LVL < 20 && SwingCount == 4)
    {
        SwingCount = 1;
    }

    if(WeaponTypePrimary == "1H" && WeaponTypeSecondary == "Bare Hands" && Attacker_LVL < 20 && SwingCount == 2)
    {
        SwingCount = 1;
    }

    if(WeaponTypePrimary == "1H" && WeaponTypeSecondary == "Bare Hands" && Attacker_LVL < 20 && SwingCount == 4)
    {
        SwingCount = 1;
    }


    //Without Weapons
    if(WeaponTypePrimary == "Bare Hands" && WeaponTypeSecondary == "Bare Hands" && Attacker_LVL >= 20 && Attacker_LVL <= 34 && SwingCount == 4)
    {
        SwingCount = 2;
    }

    if(WeaponTypePrimary == "Bare Hands" && WeaponTypeSecondary == "Bare Hands" && Attacker_LVL >= 20 && Attacker_LVL <= 34 && SwingCount == 2)
    {
        SwingCount = 2;
    }

    if(WeaponTypePrimary == "Bare Hands" && WeaponTypeSecondary == "Bare Hands" && Attacker_LVL < 20 && SwingCount == 4)
    {
        SwingCount = 1;
    }

    if(WeaponTypePrimary == "Bare Hands" && WeaponTypeSecondary == "Bare Hands" && Attacker_LVL < 20 && SwingCount == 2)
    {
        SwingCount = 1;
    }



    qDebug() << "WeaponTypeSecondary" << WeaponTypeSecondary;
    qDebug() << "WeaponTypePrimary" << WeaponTypePrimary;
    qDebug() << "SwingCount" << SwingCount;

    int rotate = 0;

    if(SwingCount != 1)
    {
        if(WeaponTypePrimary != "2H")
        {
            rotate = SwingCount / 2;
        }
        else
        {
            rotate = SwingCount;
        }
    }
    else
    {
        rotate = SwingCount;
    }

    for(int sw = 0; sw < rotate; sw++)
    {
        if(WeaponTypePrimary != "2H")
        {
            if(SwingCount != 1 || WeaponTypeSecondary == "Ranged")
            {
                typeWeaponVect.append(WeaponTypeSecondary);
                typeWeaponVect.append(SecondaryLevel);
                typeWeaponVect.append(SecondaryDamage);
            }
        }


        if(WeaponTypeSecondary != "Ranged")
        {
            typeWeaponVect.append(WeaponTypePrimary);
            typeWeaponVect.append(PrimaryLevel);
            typeWeaponVect.append(PrimaryDamage);
        }
    }

    qDebug() << "typeWeaponVect.size()" << typeWeaponVect.size()/3;

    qDebug() << "";


    int count = 0;

    for(int sw = 0; sw < typeWeaponVect.size(); sw+=3)
    {
        count++;

        WeaponType = typeWeaponVect.at(sw).toUtf8();
        WPN_LVL = typeWeaponVect.at(sw + 1).toInt(nullptr,10);
        WPN_DMG = typeWeaponVect.at(sw + 2).toInt(nullptr,10);




        float DEF_1 = Defender_LVL + Defender_Defmods;
        float DEF_2 = DEF_1 * 65;
        float DEF_3 = Defender_AC / DEF_2;

        float MAXDEF = DEF_3;


        if(MAXDEF <= .95)
        {

        }
        else
        {
            MAXDEF = .95;
        }

        MAXDEF = MAXDEF * 100;

        float DEF_4 = Defender_AGI + 300;
        float DEF_5 = DEF_4 / 1000;

        float MINDEF = DEF_3 * DEF_5;

        if(MINDEF <= .95)
        {

        }
        else
        {
            MINDEF = .95;
        }

        MINDEF = MINDEF * 100;

        float Output_1 = Attacker_LVL + Attacker_Offmods;
        float Output_2 = Output_1 + 50;
        float Output_3 = Output_2 * 10;
        float Output_4 = Attacker_STR / Output_3;
        float Output_5 = Output_1 / WPN_LVL;
        float Output_6 = WPN_DMG;
        float Output_7 = Attacker_DEX + 300;
        float Output_8 = Output_7 / 1000;

        float MAXATT = Output_4 * Output_5 * Output_6;
        float MINATT = Output_4 * Output_5 * Output_6 * Output_8;

        int _2H_Value = 0;
        int _Ranged_Value = 0;

        if(Attacker_LVL <= 19 && WeaponType == "2H")
        {
            _2H_Value = 1;
        }

        if(Attacker_LVL >= 35 && WeaponType == "2H")
        {
            _2H_Value = 2;
        }

        if(Attacker_LVL < 20 && WeaponType == "Ranged")
        {
            _Ranged_Value = 1;
        }

        if(Attacker_LVL > 19 &&  Attacker_LVL < 35 && WeaponType == "Ranged")
        {
            _Ranged_Value = 2;
        }

        if(Attacker_LVL > 34 && WeaponType == "Ranged")
        {
            _Ranged_Value = 4;
        }

        QMap<QByteArray,int> WeaponTypeMap =
        {
            {"1H",1},{"2H",_2H_Value},{"Ranged",_Ranged_Value},{"Bare Hands",1}
        };

        qDebug() << "_2H_Value" << _2H_Value;
        qDebug() << "_Ranged_Value" << _Ranged_Value;

        int Val = WeaponTypeMap.value(WeaponType);

        qDebug() << "Val" << Val;

        QVector<int> PecentVect = Levels_OffandDeff_Mods_VectorMap.value(Defender_LVL + Defender_Defmods);

        float Percent = PecentVect.at(Attacker_LVL + Attacker_Offmods);


        Percent = Percent / 100;

        MAXDEF = MAXDEF / 100;
        float Min_1 = 1 - MAXDEF;
        float Min_2 = std::ceil(MINATT * Percent * Min_1);
        int Min_3 = Min_2 + 1;

        int Min = Min_3 * Val;

        MINDEF = MINDEF / 100;
        float Max_1 = 1 - MINDEF;
        float Max_2 = std::ceil(MAXATT * Percent * Max_1);
        float Max_3 = Max_2 + 1;

        int Max = Max_3 * Val;

        //Avg
        float AVG_1 = Min + Max;
        float AVG = std::ceil(AVG_1 / 2);

        qDebug() << "Min" << Min;
        qDebug() << "Max" << Max;
        qDebug() << "AVG" << AVG;

        int output = Max;

        if(Max < 15)
        {
            Max = 15;
        }


        //output = (Max + Attacker_LVL)*2;

        std::uniform_int_distribution <int> distribution2(Min, Max);
        int RandomDamageNumber = distribution2(*QRandomGenerator::global());


        QString Damage = QString::number(RandomDamageNumber);
        qDebug() << "Dmg " + QString::number(count) + " of " + QString::number(typeWeaponVect.size()/3) + " hit for: " + Damage;

        qDebug() << "";

        DamageVector.append(Damage);

    }


    if(IN_Combat.CharSelectID == AttackersID)
    {
        IN_Combat.DamageVector = DamageVector;

    }
    else
    {
        objectpacket::IDs_DamageVectorMap.insert(AttackersID,DamageVector);
    }

    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Combat);


    qDebug() << "IN_Combat.DamageVector.size() " << IN_Combat.DamageVector.size();
    qDebug() << "DamageVector.size() " << DamageVector.size();



};



QString Combat::SendToMAXHP(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QString MyLevel1, QString MyClass1, QString MySta1)
{
    packetparsing::packetvars IN_Combat;
    IN_Combat = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    int index = ClassTypeValues.indexOf(MyClass1);
    int hpFactor = hpFactors.at(index);


    int MyLevel = MyLevel1.toInt(nullptr,16);
    int MySta = MySta1.toInt(nullptr,10);
    float MyHP = 0.0;
    int MyHP2;

    MyHP = qFloor((hpFactor + (MySta / 11)) * MyLevel);
    MyHP2 = MyHP;
    QString MyHP1 = QString("%1").arg(MyHP2,1,10,QLatin1Char('0')).toUpper();

    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Combat);

    return MyHP1;
};

QString Combat::SendToMobHP(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QString MobLevel1, QString MobClass1)
{
    packetparsing::packetvars IN_Combat;
    IN_Combat = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    int index = ClassTypeValues.indexOf(MobClass1);
    int hpFactor = hpFactors.at(index);


    int MyLevel = MobLevel1.toInt(nullptr,16);
    int MySta = MyLevel * 10;
    float MyHP = 0.0;
    int MyHP2;

    MyHP = qFloor((hpFactor + (MySta / 11)) * MyLevel);
    MyHP2 = MyHP;
    QString MyHP1 = QString("%1").arg(MyHP2,1,10,QLatin1Char('0')).toUpper();

    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Combat);

    return MyHP1;
};

QString Combat::SendToXP(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QString MyLevel1, QString MobLevel1, QString IsTargetGreen)
{
    packetparsing::packetvars IN_Combat;
    IN_Combat = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    float MyLevel = MyLevel1.toInt(nullptr,16);
    float MobLevel = MobLevel1.toInt(nullptr,16);

    //qDebug() << "MyLevel" << MyLevel;
    //qDebug() << "MobLevel" << MobLevel;

    float ExP = 0.0;
    float multiplier = 0.0;
    int ExP3 = 0;

    if(MyLevel < MobLevel)
    {
        multiplier = MobLevel - MyLevel;

        if(multiplier > 15)
        {
            multiplier = 15;
            MobLevel = MyLevel + 15; // xp cap at 10 levels higher for soloing
        }

        float multiplier1 = multiplier * 0.05;
        multiplier1 = 1.0 + multiplier1;

        ExP = 0.8* (MobLevel + 50) * (MobLevel + 1) * (MobLevel + 1);
        ExP = qFloor(multiplier1 * ExP);
        ExP3 = ExP;
    }

    if(MyLevel > MobLevel)
    {
        multiplier = MyLevel - MobLevel;
        float multiplier1 = multiplier * 0.05;
        multiplier1 = 1.0 - multiplier1;

        ExP = 0.8* (MobLevel + 50) * (MobLevel + 1) * (MobLevel + 1);
        ExP = qFloor(multiplier1 * ExP);
        ExP3 = ExP;
    }
    if(MyLevel == MobLevel)
    {
        ExP = qFloor(0.8 * (MobLevel + 50) * (MobLevel + 1) * (MobLevel + 1));
        ExP3 = ExP;
    }

    if(IsTargetGreen == "YES")// this is set up once you press the attack
    {
        ExP3 = 0;// 0 xp for green Mobs
    }

    QString ExP1 = QString("%1").arg(ExP3,1,10,QLatin1Char('0')).toUpper();

    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Combat);

    return ExP1;
};

QString Combat::SendToMAXPOW(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QString MyLevel1, QString PrimaryStat1, QString SecondaryStat1)
{
    packetparsing::packetvars IN_Combat;
    IN_Combat = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    //qDebug() << "";
    int MyLevel = MyLevel1.toInt(nullptr,16);
    //qDebug() << "MyLevel" << MyLevel;
    int PrimaryStat = PrimaryStat1.toInt(nullptr,10);
    //qDebug() << "PrimaryStat" << PrimaryStat;
    int SecondaryStat = SecondaryStat1.toInt(nullptr,10);
    //qDebug() << "SecondaryStat" << SecondaryStat;

    float MyDamage1 = PrimaryStat / 10;
    float MyDamage2 = SecondaryStat / 20;
    float MyDamage3 = MyDamage1 + MyDamage2;
    float MyDamage4 = MyDamage3 + 28;
    float MyDamage5 = MyDamage4 * MyLevel;

    int MyDamage6 = MyDamage5;

    //qDebug() << "MaxPower1" << MyDamage6;

    QString MyDamage7 = QString("%1").arg(MyDamage6,1,10,QLatin1Char('0')).toUpper();
    //qDebug() << "MaxPower2" << MyDamage7;

    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Combat);
    return MyDamage7;
};
