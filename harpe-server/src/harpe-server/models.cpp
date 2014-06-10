#include <harpe-server/models.hpp>
/****************************************
 ****************** DATAS ***************
 ***************************************/

/***************** AA ***********************/
REGISTER_AND_CONSTRUCT(AA,"website_aa",slug,"slug",mass,"mass");


ntw::Serializer& AA::serialize(ntw::Serializer& stream,orm::DB& db)
{
    stream<<this->pk
        <<this->slug
        <<this->mass;
    return stream;
}

/*************** AA Modification ***********************************/
REGISTER_AND_CONSTRUCT(AAModification,"website_aamodification",name,"name",delta,"delta");

ntw::Serializer& AAModification::serialize(ntw::Serializer& stream,orm::DB& db)
{
    //AAModificationPosition
    std::list<orm::Cache<AAModificationPosition>::type_ptr> aamodificationposition;
    AAModificationPosition::query(db)\
        .filter(this->pk,"exact",AAModificationPosition::_modification)\
        .get(aamodificationposition);

    stream<<this->pk
        <<this->delta
        <<(unsigned int)aamodificationposition.size();

    for (auto& aamod : aamodificationposition)
        aamod->serialize(stream,db);

    return stream;
}

/***************** AA Modification Position *********************/
REGISTER_AND_CONSTRUCT(AAModificationPosition,"website_aamodificationposition",aa,"AA_id",modification,"modification_id",position,"position");

ntw::Serializer& AAModificationPosition::serialize(ntw::Serializer& stream,orm::DB& db)
{
    stream<<this->pk;
    this->aa->serialize(stream,db);
        //<<self.modification
    stream<<this->position;
    return stream;
}


/***************** Enzyme *************************/
REGISTER_AND_CONSTRUCT(Enzyme,"website_enzyme",name,"name");

/**************** ANALYSEMGF *******************/
//REGISTER_AND_CONSTRUCT(AnalyseMgf,"website_analysemgf",mgf,"mgf");
M2M_REGISTER(AnalyseMgf,AAs,AA,"website_analysemgf_AAs","analysemgf_id","aa_id")
M2M_REGISTER(AnalyseMgf,modification,AAModification,"website_analysemgf_modifications","analysemgf_id","aamodification_id")
REGISTER(AnalyseMgf,"website_analysemgf",enzyme,"enzyme_id",mgf,"mgf",max_charge,"max_charge",error,"error")
AnalyseMgf::AnalyseMgf() : enzyme(AnalyseMgf::_enzyme),mgf(AnalyseMgf::_mgf),max_charge(AnalyseMgf::_max_charge),error(AnalyseMgf::_error),AAs(*this), modifications(*this)
{
    enzyme.registerAttr(*this);
    mgf.registerAttr(*this);
    max_charge.registerAttr(*this);
    error.registerAttr(*this);
}

ntw::Serializer& AnalyseMgf::serialize(ntw::Serializer& stream,orm::DB& db)
{
    stream<<this->pk
        <<this->max_charge
        <<this->error;
    // AAs
    {
        auto aas = this->AAs.all(db);
        stream<<(unsigned int)aas.size();
        for(auto& aa_ptr: aas)
            aa_ptr->serialize(stream,db);
    }

    //modifications PTMs
    {
        auto modifications = this->modifications.all(db);
        stream<<(unsigned int)modifications.size();

        for(auto& mod : modifications)
            mod->serialize(stream,db);
    }

    return stream;
}


/********************** AnalysePeptide ******************************/
REGISTER_AND_CONSTRUCT(AnalysePeptide,"website_analysepeptide",analyse,"analyse_id",name,"name",mz,"mz",mass,"mass",intensity,"intensity",charge,"charge",mgf_part,"mgf_part",cmpd,"cmpd",status,"status");

ntw::Serializer& AnalysePeptide::serialize(ntw::Serializer& stream,orm::DB& db)
{
    stream<<this->pk
        <<this->mgf_part;
    return this->analyse->serialize(stream,db);
}

M2M_REGISTER(AnalysePeptideValidated,modifications,AAModification,"website_analysepeptidevalidated_modifications","Analysepeptidevalidate_id","aamodification_id")
REGISTER(AnalysePeptideValidated,"website_analysepeptidevalidated",score,"score",analyse,"analyse_id",sequence,"sequence",modification_seq,"modification_seq")
AnalysePeptideValidated::AnalysePeptideValidated() : score(AnalysePeptideValidated::_score), analyse(AnalysePeptideValidated::_analyse), sequence(AnalysePeptideValidated::_sequence), modification_seq(AnalysePeptideValidated::_modification_seq),modifications(*this)
{
    score.registerAttr(*this);
    analyse.registerAttr(*this);
    sequence.registerAttr(*this);
    modification_seq.registerAttr(*this);
}

/******************** CalculatedPeptide *****************************/
REGISTER_AND_CONSTRUCT(CalculatedPeptide,"website_calculatedpeptide",score,"score",\
                       sequence,"sequence",\
                       analyse,"analyse_id",\
                       error_total,"error_total",\
                       error_aa_cumul,"error_aa_cumul",\
                       intensitee_total_parent,"intensitee_total_parent",\
                       intensitee_total,"intensitee_total",
                       mass_total,"mass_total",\
                       percent_couverture,"percent_couverture",\
                       nb_aa,"nb_aa",\
                       nb_peaks,"nb_peaks",\
                       mass_parent,"mass_parent",\
                       percent_intensitee_utilisee,"percent_intensitee_utilisee");

/**********************************************************
 ******************** COMMUNICATION ***********************
 *********************************************************/

/************** Server ***************/
REGISTER_AND_CONSTRUCT(HarpeServer,"communication_harpeserver",name,"name",ip,"ip",port,"port",is_active,"is_active");


/*************** Client ***************/
REGISTER_AND_CONSTRUCT(Client,"communication_client",ip,"ip",port,"port",server,"server_id",is_active,"is_active");

/*************** M2M Client->AnalysePeptide *************/
REGISTER_AND_CONSTRUCT(ClientCalculation,"communication_clientcalculation",client,"client_id",analysepeptide,"analysepeptide_id",status,"status");

