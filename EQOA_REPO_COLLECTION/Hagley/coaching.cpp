#include "coaching.h"
#include "packetparsing.h"
#include "opcodes.h"

Coaching::Coaching(QObject *parent) : QObject(parent)
{

}
QByteArray Coaching::coach(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QString coach_id) //takes in coachman id spits out 3400 packet
{
    packetparsing::packetvars IN_coach;
    IN_coach = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    merged = ""; //clear at the top since we will be called from mainwindow
    coach_order.clear();
    IN_coach.ellisto.clear();
    IN_coach.destination.clear();
    IN_coach.swapnames.clear();
    swapmap.clear();
    IN_coach.positionlist.clear();

    //qDebug() << "coach_id" << coach_id;
    //qDebug() << "IN_coach.CharSelectID" << IN_coach.CharSelectID;

    QVector<QVector<QString>> MainCoachVector = Opcodes::ID_VectorOfCoachesVectorMap.value(IN_coach.CharSelectID);

    IN_coach.ellisto = MainCoachVector.at(0);
    IN_coach.positionlist = MainCoachVector.at(1);



    QByteArray coachbyte_id = coach_id.toUtf8(); //swap to bytearray
    qlonglong coachid_int = coachbyte_id.toLongLong(nullptr,16); //change to int

    //qDebug() << "IN_coach.ellisto" << IN_coach.ellisto;
    //qDebug() << "IN_coach.positionlist" << IN_coach.positionlist;

    switch (coachid_int)
    {
    case  0xDEB20200: //coachman ronks freeport
        swapmap = freeport_map; //19,34, 4, 44, 48 //would write 19 = 0, 20 = 1 after signed
        IN_coach.swapnames = FP_Names;
        free_a.append(IN_coach.ellisto.value(19));
        free_a.append(IN_coach.ellisto.value(34));
        free_a.append(IN_coach.ellisto.value(4));
        free_a.append(IN_coach.ellisto.value(44));
        free_a.append(IN_coach.ellisto.value(48));

        coach_order = free_a.toUtf8();
        //qDebug() << "IN_coach.swapnames" << IN_coach.swapnames;
        free_a.clear();

        break;

    case 0xDEB50200: //coachman boblon bobble
        swapmap = bobble_map; // 3,20,38,54 //would write 3 = 0, 4 = 1 after signed
        IN_coach.swapnames = bobble_names;
        bobble_a.append(IN_coach.ellisto.value(3));
        bobble_a.append(IN_coach.ellisto.value(20));
        bobble_a.append(IN_coach.ellisto.value(38));
        bobble_a.append(IN_coach.ellisto.value(56));

        coach_order = bobble_a.toUtf8();
        ////qDebug() << "coach order bobble" << coach_order;
        bobble_a.clear();
        break;
    case 0x56E90000: //coachman dapkus blackwater
        swapmap = blackwater_map;
        IN_coach.swapnames = blackwater_names;
        blackwater_a.append(IN_coach.ellisto.value(1));
        blackwater_a.append(IN_coach.ellisto.value(50));
        blackwater_a.append(IN_coach.ellisto.value(22));
        blackwater_a.append(IN_coach.ellisto.value(16));

        coach_order = blackwater_a.toUtf8();
        ////qDebug() << "coach order blackwater" << coach_order;
        blackwater_a.clear();
        break;

    case 0x57E90000: //coachman brody clw
        swapmap = castle_lightwolf_map;
        IN_coach.swapnames = castle_lightwolf_names;
        castle_lightwolf_a.append(IN_coach.ellisto.value(5));
        castle_lightwolf_a.append(IN_coach.ellisto.value(26));
        castle_lightwolf_a.append(IN_coach.ellisto.value(46));

        coach_order = castle_lightwolf_a.toUtf8();
        ////qDebug() << "coach order clw" << coach_order;
        castle_lightwolf_a.clear();

        break;

    case 0x54E70000: //coachman nioclas dark solace
        swapmap = dark_solace_map;
        IN_coach.swapnames = dark_solace_names;
        dark_solace_a.append(IN_coach.ellisto.value(7));
        dark_solace_a.append(IN_coach.ellisto.value(34));
        dark_solace_a.append(IN_coach.ellisto.value(50));
        dark_solace_a.append(IN_coach.ellisto.value(36));
        dark_solace_a.append(IN_coach.ellisto.value(52));
        dark_solace_a.append(IN_coach.ellisto.value(14));

        coach_order = dark_solace_a.toUtf8();
        ////qDebug() << "coach order dark solace" << coach_order;
        dark_solace_a.clear();

        break;

    case 0x58E90000: //coachman darva darvar manor
        swapmap = darvar_manor_map;
        IN_coach.swapnames = darvar_manor_names;
        darvar_manor_a.append(IN_coach.ellisto.value(9));
        darvar_manor_a.append(IN_coach.ellisto.value(40));
        darvar_manor_a.append(IN_coach.ellisto.value(34));
        darvar_manor_a.append(IN_coach.ellisto.value(16));
        darvar_manor_a.append(IN_coach.ellisto.value(58));

        coach_order = darvar_manor_a.toUtf8();
        ////qDebug() << "coach order darvar manor" << coach_order;
        darvar_manor_a.clear();
        break;

    case 0x59E90000:           //coachman thirtreel fayspires
        swapmap = fayspires_map;
        IN_coach.swapnames = fayspires_names;
        fayspires_a.append(IN_coach.ellisto.value(11));
        fayspires_a.append(IN_coach.ellisto.value(38));
        fayspires_a.append(IN_coach.ellisto.value(60));
        fayspires_a.append(IN_coach.ellisto.value(56));
        fayspires_a.append(IN_coach.ellisto.value(40));

        coach_order = fayspires_a.toUtf8();
        ////qDebug() << "coach order fayspires" << coach_order;
        fayspires_a.clear();
        break;

    case 0xAFE70000: //Coachman Norial fog marsh/honjour
        swapmap = fog_marsh_map;
        IN_coach.swapnames = fog_marsh_names;
        fog_marsh_a.append(IN_coach.ellisto.value(13));
        fog_marsh_a.append(IN_coach.ellisto.value(42));
        fog_marsh_a.append(IN_coach.ellisto.value(8));
        fog_marsh_a.append(IN_coach.ellisto.value(62));

        coach_order = fog_marsh_a.toUtf8();
        ////qDebug() << "coach order fog marsh/honjour" << coach_order;
        fog_marsh_a.clear();
        break;

    case 0x5AE90000:  //Coachwoman Holly forkwatch
        swapmap = forkwatch_map;
        IN_coach.swapnames = forkwatch_names;
        forkwatch_a.append(IN_coach.ellisto.value(15));
        forkwatch_a.append(IN_coach.ellisto.value(10));
        forkwatch_a.append(IN_coach.ellisto.value(2));
        forkwatch_a.append(IN_coach.ellisto.value(32));
        forkwatch_a.append(IN_coach.ellisto.value(54));

        coach_order = forkwatch_a.toUtf8();
        ////qDebug() << "coach order forkwatch" << coach_order;
        forkwatch_a.clear();
        break;

    case 0x5BE90000: //Coachman D`larik  fort seriak
        swapmap = fort_seriak_map;
        IN_coach.swapnames = fort_seriak_names;
        fort_seriak_a.append(IN_coach.ellisto.value(17));
        fort_seriak_a.append(IN_coach.ellisto.value(48));
        fort_seriak_a.append(IN_coach.ellisto.value(42));
        fort_seriak_a.append(IN_coach.ellisto.value(34));

        coach_order = fort_seriak_a.toUtf8();
        ////qDebug() << "coach order fort seriak" << coach_order;
        fort_seriak_a.clear();
        break;

    case 0x5CE90000: //Coachman Ungar gerntar mines
        swapmap = gerntar_mines_map;
        IN_coach.swapnames = gerntar_mines_names;
        gerntar_mines_a.append(IN_coach.ellisto.value(21));
        gerntar_mines_a.append(IN_coach.ellisto.value(2));

        coach_order = gerntar_mines_a.toUtf8();
        ////qDebug() << "coach order gerntar mines" << coach_order;
        gerntar_mines_a.clear();
        break;

    case 0x63E60000: //Coachman Zuggug grobb
        swapmap = grobb_map;
        IN_coach.swapnames = grobb_names;
        grobb_a.append(IN_coach.ellisto.value(23));
        grobb_a.append(IN_coach.ellisto.value(28));
        grobb_a.append(IN_coach.ellisto.value(36));

        coach_order = grobb_a.toUtf8();
        ////qDebug() << "coach order grobb" << coach_order;
        grobb_a.clear();
        break;

    case 0x5DE90000: //Coachman Dudley halas
        swapmap = halas_map;
        IN_coach.swapnames = halas_names;
        halas_a.append(IN_coach.ellisto.value(25));
        halas_a.append(IN_coach.ellisto.value(40));
        halas_a.append(IN_coach.ellisto.value(6));

        coach_order = halas_a.toUtf8();
        ////qDebug() << "coach order halas" << coach_order;
        halas_a.clear();
        break;

    case 0x5EE90000: //Coachman Droogra hazinak dock
        swapmap = hazinak_dock_map;
        IN_coach.swapnames = hazinak_dock_names;
        hazinak_dock_a.append(IN_coach.ellisto.value(27));
        hazinak_dock_a.append(IN_coach.ellisto.value(50));
        hazinak_dock_a.append(IN_coach.ellisto.value(24));

        coach_order = hazinak_dock_a.toUtf8();
        ////qDebug() << "coach order haz dock" << coach_order;
        hazinak_dock_a.clear();
        break;

    case 0x5FE90000: //Coachman Zelik hazinak temple
        swapmap = hazinak_temple_map;
        IN_coach.swapnames = hazinak_temple_names;
        hazinak_temple_a.append(IN_coach.ellisto.value(29));
        hazinak_temple_a.append(IN_coach.ellisto.value(50));

        coach_order = hazinak_temple_a.toUtf8();
        ////qDebug() << "coach order haz temple" << coach_order;
        hazinak_temple_a.clear();
        break;

    case 0xDBE80000: //Coachman Galdah highbourne
        swapmap = highbourne_map;
        IN_coach.swapnames = highbourne_names;
        highbourne_a.append(IN_coach.ellisto.value(31));
        highbourne_a.append(IN_coach.ellisto.value(54));
        highbourne_a.append(IN_coach.ellisto.value(16));

        coach_order = highbourne_a.toUtf8();
        ////qDebug() << "coach order highbourne" << coach_order;
        highbourne_a.clear();
        break;

    case 0xDEB10200: //Coachman Quicksteed highpass
        swapmap = highpass_map;
        IN_coach.swapnames = highpass_names;
        highpass_a.append(IN_coach.ellisto.value(33));
        highpass_a.append(IN_coach.ellisto.value(56));
        highpass_a.append(IN_coach.ellisto.value(10));
        highpass_a.append(IN_coach.ellisto.value(20));
        highpass_a.append(IN_coach.ellisto.value(8));
        highpass_a.append(IN_coach.ellisto.value(18));

        coach_order = highpass_a.toUtf8();
        ////qDebug() << "coach order highpass" << coach_order;
        highpass_a.clear();
        break;

    case 0x61E90000: //Saca the Horse Keeper kerplunk
        swapmap = kerplunk_map;
        IN_coach.swapnames = kerplunk_names;
        kerplunk_a.append(IN_coach.ellisto.value(35));
        kerplunk_a.append(IN_coach.ellisto.value(24));
        kerplunk_a.append(IN_coach.ellisto.value(52));
        kerplunk_a.append(IN_coach.ellisto.value(8));

        coach_order = kerplunk_a.toUtf8();
        ////qDebug() << "coach order kerplunk" << coach_order;
        kerplunk_a.clear();
        break;

    case 0x62E90000: //Coachman Rizkar klickanon
        swapmap = klickanon_map;
        IN_coach.swapnames = klickanon_names;
        klickanon_a.append(IN_coach.ellisto.value(37));
        klickanon_a.append(IN_coach.ellisto.value(4));
        klickanon_a.append(IN_coach.ellisto.value(12));
        klickanon_a.append(IN_coach.ellisto.value(48));

        coach_order = klickanon_a.toUtf8();
        ////qDebug() << "coach order klickanon" << coach_order;
        klickanon_a.clear();
        break;

    case 0xE3E80000: //Coachman Doba moradhim
        swapmap = moradhim_map;
        IN_coach.swapnames = moradhim_names;
        moradhim_a.append(IN_coach.ellisto.value(39));
        moradhim_a.append(IN_coach.ellisto.value(12));
        moradhim_a.append(IN_coach.ellisto.value(26));
        moradhim_a.append(IN_coach.ellisto.value(10));
        moradhim_a.append(IN_coach.ellisto.value(56));

        coach_order = moradhim_a.toUtf8();
        ////qDebug() << "coach order moradhim" << coach_order;
        moradhim_a.clear();
        break;

    case 0x60E90000: //Coachman Opelim mt hatespike /last inn
        swapmap = hatespike_map;
        IN_coach.swapnames = hatespike_names;
        hatespike_a.append(IN_coach.ellisto.value(41));
        hatespike_a.append(IN_coach.ellisto.value(18));
        hatespike_a.append(IN_coach.ellisto.value(14));

        coach_order = hatespike_a.toUtf8();
        ////qDebug() << "coach order mt hatespike/last inn" << coach_order;
        hatespike_a.clear();
        break;

    case 0xDEB60200: //Coachman Farn mtg
        swapmap = muniels_tea_garden_map;
        IN_coach.swapnames = muniels_tea_garden_names;
        muniels_tea_garden_a.append(IN_coach.ellisto.value(43));
        muniels_tea_garden_a.append(IN_coach.ellisto.value(20));
        muniels_tea_garden_a.append(IN_coach.ellisto.value(50));

        coach_order = muniels_tea_garden_a.toUtf8();
        ////qDebug() << "coach order mtg" << coach_order;
        muniels_tea_garden_a.clear();
        break;

    case 0x09E90000: //Coachwoman Dainei murnf
        swapmap = murnf_map;
        IN_coach.swapnames = murnf_names;
        murnf_a.append(IN_coach.ellisto.value(45));
        murnf_a.append(IN_coach.ellisto.value(6));
        murnf_a.append(IN_coach.ellisto.value(62));

        coach_order = murnf_a.toUtf8();
        ////qDebug() << "coach order murnf" << coach_order;
        murnf_a.clear();
        break;

    case 0xDFB10200: //Coachman Lothi neriak
        swapmap = neriak_map;
        IN_coach.swapnames = neriak_names;
        neriak_a.append(IN_coach.ellisto.value(47));
        neriak_a.append(IN_coach.ellisto.value(20));
        neriak_a.append(IN_coach.ellisto.value(38));
        neriak_a.append(IN_coach.ellisto.value(18));

        coach_order = neriak_a.toUtf8();
        ////qDebug() << "coach order neriak" << coach_order;
        neriak_a.clear();
        break;

    case 0x63E90000: //Coachwoman Dernia oasis
        swapmap = oasis_map;
        IN_coach.swapnames = oasis_names;
        oasis_a.append(IN_coach.ellisto.value(49));
        oasis_a.append(IN_coach.ellisto.value(30));
        oasis_a.append(IN_coach.ellisto.value(2));
        oasis_a.append(IN_coach.ellisto.value(44));
        oasis_a.append(IN_coach.ellisto.value(28));
        oasis_a.append(IN_coach.ellisto.value(8));

        coach_order = oasis_a.toUtf8();
        ////qDebug() << "coach order oasis" << coach_order;
        oasis_a.clear();
        break;

    case 0x64E90000: //Coachman Dilga oggok
        swapmap = oggok_map;
        IN_coach.swapnames = oggok_names;
        oggok_a.append(IN_coach.ellisto.value(51));
        oggok_a.append(IN_coach.ellisto.value(8));
        oggok_a.append(IN_coach.ellisto.value(36));

        coach_order = oggok_a.toUtf8();
        ////qDebug() << "coach order oggok" << coach_order;
        oggok_a.clear();
        break;

    case 0x11E90000: //Coachman Billfer qeynos
        swapmap = qeynos_map;
        IN_coach.swapnames = qeynos_names;
        qeynos_a.append(IN_coach.ellisto.value(53));
        qeynos_a.append(IN_coach.ellisto.value(62));
        qeynos_a.append(IN_coach.ellisto.value(58));
        qeynos_a.append(IN_coach.ellisto.value(16));
        qeynos_a.append(IN_coach.ellisto.value(32));

        coach_order = qeynos_a.toUtf8();
        ////qDebug() << "coach order qeynos" << coach_order;
        qeynos_a.clear();
        break;

    case 0x65E90000: //Coachman Huff rivervale
        swapmap = rivervale_map;
        IN_coach.swapnames = rivervale_names;
        rivervale_a.append(IN_coach.ellisto.value(55));
        rivervale_a.append(IN_coach.ellisto.value(12));
        rivervale_a.append(IN_coach.ellisto.value(4));
        rivervale_a.append(IN_coach.ellisto.value(34));
        rivervale_a.append(IN_coach.ellisto.value(40));

        coach_order = rivervale_a.toUtf8();
        ////qDebug() << "coach order rivervale" << coach_order;
        rivervale_a.clear();
        break;

    case 0x53E90000: //Coachman Frender surefall glade
        swapmap = surefall_glade_map;
        IN_coach.swapnames = surefall_glade_names;
        surefall_glade_a.append(IN_coach.ellisto.value(57));
        surefall_glade_a.append(IN_coach.ellisto.value(10));
        surefall_glade_a.append(IN_coach.ellisto.value(62));
        surefall_glade_a.append(IN_coach.ellisto.value(54));

        coach_order = surefall_glade_a.toUtf8();
        ////qDebug() << "coach order surefall glade" << coach_order;
        surefall_glade_a.clear();
        break;

    case 0x66E90000: //Coachman Zaris Tethelin
        swapmap = tethelin_map;
        IN_coach.swapnames = tethelin_names;
        tethelin_a.append(IN_coach.ellisto.value(59));
        tethelin_a.append(IN_coach.ellisto.value(12));

        coach_order = tethelin_a.toUtf8();
        ////qDebug() << "coach order tethelin" << coach_order;
        tethelin_a.clear();
        break;

    case 0x57E70000: //Coachman Brumley wyndhaven
        swapmap = wyndhaven_map;
        IN_coach.swapnames = wyndhaven_names;
        wyndhaven_a.append(IN_coach.ellisto.value(61));
        wyndhaven_a.append(IN_coach.ellisto.value(46));
        wyndhaven_a.append(IN_coach.ellisto.value(58));
        wyndhaven_a.append(IN_coach.ellisto.value(54));
        wyndhaven_a.append(IN_coach.ellisto.value(14));
        wyndhaven_a.append(IN_coach.ellisto.value(64));

        coach_order = wyndhaven_a.toUtf8();
        ////qDebug() << "coach order wyndhaven" << coach_order;
        wyndhaven_a.clear();
        break;

    case 0x58E70000: //coachman skeel zentars keep
        swapmap = zentars_keep_map;
        IN_coach.swapnames = zentars_keep_names;
        zentars_keep_a.append(IN_coach.ellisto.value(63));
        zentars_keep_a.append(IN_coach.ellisto.value(62));

        coach_order = zentars_keep_a.toUtf8();
        ////qDebug() << "coach order zentars keep" << coach_order;
        zentars_keep_a.clear();
        break;




    default:
        QByteArray nomatch = "NULL";
        return nomatch; //its not a coachman, so return out
        break;
    }

    int size1 = coach_order.size();

    //qDebug() << "IN_coach.coach_order" << coach_order;

    for (int cd = 0; cd < size1; cd++) //rotate from 0 to whatever
    {
        if(coach_order.mid(cd,1) == "1") //if any of the values is 1
        {
            ////qDebug() << "one" << size1;
            QByteArray output1 = swapmap.value(cd); //get the value from the map
            ////qDebug() <<"output1" <<output1;
            merged += output1; //add it to a bytearray
            ////qDebug() << "CD" << cd;
            IN_coach.destination.append(cd);
        }
    }

    //qDebug() << "IN_coach.destination" << IN_coach.destination;


    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_coach);

    ////qDebug() << "IN_coach.destination" << IN_coach.destination;
    QString numoptions1 = QString("%1").arg(IN_coach.destination.count(),2,10,QLatin1Char ('0'));
    if(IN_coach.destination.count() == 0)
    {
        QByteArray noout1 = NoCoaches;
        int newfbsize = noout1.size() / 2 + 6;
        QString newfbhex = QString::number(newfbsize,16);
        QByteArray nosizehex1 = newfbhex.toUtf8();
        QByteArray nextnoheader = Header2;
        QByteArray nocoachout = nextnoheader + NoCoaches;
        merged = ""; //clear merged
        coach_order.clear();
        IN_coach.ellisto.clear();
        return nocoachout;     //return from here if no coaches
    }

    QByteArray optionsnum1 = numoptions1.toUtf8();
    QByteArray Out1 =  Greeting + optionsnum1 + merged; //dont add header to get count
    int currentfbsize = Out1.size() / 2 + 6;   //6 bytes from 3400 opcode to count 0100 0000
    QString currentfbhex = QString::number(currentfbsize,16);
    QByteArray sizehex1 = currentfbhex.toUtf8();

    if(currentfbsize <= 255 && IN_coach.destination.size() != 0) //less than 255 small header
    {
        QByteArray newheader1 = Header2;
        QByteArray smallcoachout1 = newheader1.toUpper() + Greeting + optionsnum1 + merged;
        merged = ""; //clear merged
        coach_order.clear();
        IN_coach.ellisto.clear();
        return smallcoachout1;
    }
    if(currentfbsize > 255 && IN_coach.destination.size() != 0)
    {
        QString paddedhex1 = currentfbhex;
        paddedhex1.insert(0,"0"); //adding 0 to the beginning
        QString firststring = paddedhex1.mid(0,2);
        QString secondstring = paddedhex1.mid(2,2);   //poor mans bit shift
        QString outrev1 = secondstring + firststring;
        QByteArray newlargeheader = Header;
        QByteArray largecoachout1 = newlargeheader.toUpper() + Greeting + optionsnum1 + merged;
        merged = ""; //clear merged
        coach_order.clear();
        IN_coach.ellisto.clear();
        return largecoachout1;
    }

    QByteArray extranull = "NULL";
    return extranull;



}

QByteArray Coaching::leaving(QString coach_id,int client_answer) //need to save coach_id from 0400
{
    QByteArray coachbyte_id2 = coach_id.toUtf8(); //swap to bytearray
    qlonglong coachid_int2 = coachbyte_id2.toLongLong(0,16); //change to int

    switch (coachid_int2)
    {

    case 0xDEB20200: //coachman ronks freeport
        if(client_answer == 0)
        {
            main_text_return = Freeport_sign;
        }
        else
        {
            main_text_return = Freeport_reply1;
        }

        break;


    case 0xDEB50200: //coachman boblon bobble
        if(client_answer == 0)
        {
            main_text_return = Bobble_sign;
        }
        else
        {
            main_text_return = Bobble_reply1;
        }
        break;
    case 0x56E90000: //coachman dapkus blackwater
        if(client_answer == 0)
        {
            main_text_return = Blackwater_sign;
        }
        else
        {
            main_text_return = Blackwater_reply1;
        }

        break;
    case 0x57E90000: //coachman brody clw
        if(client_answer == 0)
        {
            main_text_return = Castle_Lightwolf_sign;
        }
        else
        {
            main_text_return = Castle_Lightwolf_reply1;
        }
        break;
    case 0x54E70000: //coachman nioclas dark solace
        if(client_answer == 0)
        {
            main_text_return = Dark_Solace_sign;
        }
        else
        {
            main_text_return = Dark_Solace_reply1;
        }
        break;
    case 0x58E90000: //coachman darva darvar manor
        if(client_answer == 0)
        {
            main_text_return = Darva_Manor_sign;
        }
        else
        {
            main_text_return = Darva_Manor_reply1;
        }
        break;
    case 0x59E90000:   //coachman thirtreel fayspires
        if(client_answer == 0)
        {
            main_text_return = Fayspires_sign;
        }
        else
        {
            main_text_return = Fayspires_reply1;
        }
        break;
    case 0xAFE70000: //Coachman Norial fog marsh/honjour
        if(client_answer == 0)
        {
            main_text_return = Fog_Marsh_sign;
        }
        else
        {
            main_text_return = Fog_Marsh_reply1;
        }
        break;
    case 0x5AE90000:  //Coachwoman Holly forkwatch
        if(client_answer == 0)
        {
            main_text_return = Forkwatch_sign;
        }
        else
        {
            main_text_return = Forkwatch_reply1;
        }
        break;
    case 0x5BE90000: //Coachman D`larik  fort seriak
        if(client_answer == 0)
        {
            main_text_return = Fort_Seriak_sign;
        }
        else
        {
            main_text_return = Fort_Seriak_reply1;
        }
        break;
    case 0x5CE90000: //Coachman Ungar gerntar mines
        if(client_answer == 0)
        {
            main_text_return  = Gerntar_Mines_sign;
        }
        else
        {
            main_text_return = Gerntar_Mines_reply1;
        }
        break;
    case 0x63E60000: //Coachman Zuggug grobb
        if(client_answer == 0)
        {
            main_text_return = Grobb_sign;
        }
        else
        {
            main_text_return = Grobb_reply1;
        }
        break;
    case 0x5DE90000: //Coachman Dudley halas
        if(client_answer == 0)
        {
            main_text_return = Halas_sign;
        }
        else
        {
            main_text_return = Halas_reply1;
        }
        break;
    case 0x5EE90000: //Coachman Droogra hazinak dock
        if(client_answer == 0)
        {
            main_text_return = Hazinak_Dock_sign;
        }
        else
        {
            main_text_return = Hazinak_Dock_reply1;
        }
        break;
    case 0x5FE90000: //Coachman Zelik hazinak temple
        if(client_answer == 0)
        {
            main_text_return = Hazinak_Temple_sign;
        }
        else
        {
            main_text_return = Hazinak_Temple_reply1;
        }
        break;
    case 0xDBE80000: //Coachman Galdah highbourne
        if(client_answer == 0)
        {
            main_text_return = Highbourne_sign;
        }
        else
        {
            main_text_return = Highbourne_reply1;
        }
        break;
    case 0xDEB10200: //Coachman Quicksteed highpass
        if(client_answer == 0)
        {
            main_text_return = Highpass_sign;
        }
        else
        {
            main_text_return = Highpass_reply1;
        }
        break;
    case 0x61E90000: //Saca the Horse Keeper kerplunk
        if(client_answer == 0)
        {
            main_text_return = Kerplunk_sign;
        }
        else
        {
            main_text_return = Kerplunk_reply1;
        }
        break;
    case 0x62E90000: //Coachman Rizkar klickanon
        if(client_answer == 0)
        {
            main_text_return = Klickanon_sign;
        }
        else
        {
            main_text_return = Klickanon_reply1;
        }
        break;
    case 0xE3E80000: //Coachman Doba moradhim
        if(client_answer == 0)
        {
            main_text_return = Moradhim_sign;
        }
        else
        {
            main_text_return = Moradhim_reply1;
        }
        break;
    case 0x60E90000: //Coachman Opelim mt hatespike /last inn
        if(client_answer == 0)
        {
            main_text_return = Hatespike_sign;
        }
        else
        {
            main_text_return = Hatespike_reply1;
        }
        break;
    case 0xDEB60200: //Coachman Farn mtg
        if(client_answer == 0)
        {
            main_text_return = Muniels_Tea_Garden_sign;
        }
        else
        {
            main_text_return = Muniels_Tea_Garden_reply1;
        }
        break;
    case 0x09E90000: //Coachwoman Dainei murnf
        if(client_answer == 0)
        {
            main_text_return = Murnf_sign;
        }
        else
        {
            main_text_return = Murnf_reply1;
        }
        break;
    case 0xDFB10200: //Coachman Lothi neriak
        if(client_answer == 0)
        {
            main_text_return = Neriak_sign;
        }
        else
        {
            main_text_return = Neriak_reply1;
        }
        break;
    case 0x63E90000: //Coachwoman Dernia oasis
        if(client_answer == 0)
        {
            main_text_return = Oasis_sign;
        }
        else
        {
            main_text_return = Oasis_reply1;
        }
        break;
    case 0x64E90000: //Coachman Dilga oggok
        if(client_answer == 0)
        {
            main_text_return = Oggok_sign;
        }
        else
        {
            main_text_return = Oggok_reply1;
        }
        break;
    case 0x11E90000: //Coachman Billfer qeynos
        if(client_answer == 0)
        {
            main_text_return = Qeynos_sign;
        }
        else
        {
            main_text_return = Qeynos_reply1;
        }
        break;
    case 0x65E90000: //Coachman Huff rivervale
        if(client_answer == 0)
        {
            main_text_return = Rivervale_sign;
        }
        else
        {
            main_text_return = Rivervale_reply1;
        }
        break;
    case 0x53E90000: //Coachman Frender surefall glade
        if(client_answer == 0)
        {
            main_text_return = Surefall_Glade_sign;
        }
        else
        {
            main_text_return = Surefall_Glade_reply1;
        }
        break;
    case 0x66E90000: //Coachman Zaris Tethelin
        if(client_answer == 0)
        {
            main_text_return = Tethelin_sign;
        }
        else
        {
            main_text_return = Tethelin_reply1;
        }
        break;
    case 0x57E70000: //Coachman Brumley wyndhaven
        if(client_answer == 0)
        {
            main_text_return = Wyndhaven_sign;
        }
        else
        {
            main_text_return = Wyndhaven_reply1;
        }
        break;
    case 0x58E70000: //coachman skeel zentars keep
        if(client_answer == 0)
        {
            main_text_return = Zentars_Keep_sign;
        }
        else
        {
            main_text_return = Zentars_Keep_reply1;
        }
        break;

    default:
        QByteArray supernull1 = "NULL";
        return supernull1;
        break;
    }

    int currentfbsize2 = main_text_return.size() / 2 + 6;   //6 bytes from 4600 opcode to count 0100 0000
    QString currentfbhex2 = QString::number(currentfbsize2,16);

    QByteArray newheader2 = Leave_Header2;
    QByteArray smallcoachout2 = newheader2 + main_text_return;
    return smallcoachout2;
}
