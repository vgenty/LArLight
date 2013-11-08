#ifndef TIMEQCTABLE_CC
#define TIMEQCTABLE_CC

#include "TimeQCTable.hh"

TimeQCTable::TimeQCTable() : QCTableBase() 
{ 
  _tablename="TimeQCTable"; 
  Initialize(); 
}

const std::string TimeQCTable::GetTableDef(DB::DB_t type) const
{
  std::string table_def = Form("%s SMALLINT NOT NULL,",QC::kQCFieldName[QC::kMonKey].c_str());
  table_def += Form("%s TIMESTAMP NOT NULL,",QC::kQCFieldName[QC::kTimeBegin].c_str());
  table_def += Form("%s TIMESTAMP NULL,",QC::kQCFieldName[QC::kTimeEnd].c_str());
  switch(type){
  case DB::kMySQL:
    table_def += Form("%s DOUBLE NOT NULL,",QC::kQCFieldName[QC::kMean].c_str());
    table_def += Form("%s DOUBLE NOT NULL,",QC::kQCFieldName[QC::kSigma].c_str());
    break;
  case DB::kPostgreSQL:
    table_def += Form("%s DOUBLE PRECISION NOT NULL,",QC::kQCFieldName[QC::kMean].c_str());
    table_def += Form("%s DOUBLE PRECISION NOT NULL,",QC::kQCFieldName[QC::kSigma].c_str());
    break;
  }
  table_def += Form("%s VARCHAR(100) NOT NULL,",QC::kQCFieldName[QC::kReference].c_str());
  table_def += Form("%s TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,",QC::kQCFieldName[QC::kLogTime].c_str());
  table_def += Form("KEY %s, KEY %s, KEY %s, PRIMARY KEY (%s, %s, %s)",
		    QC::kQCFieldName[QC::kMonKey].c_str(),
		    QC::kQCFieldName[QC::kTimeBegin].c_str(),
		    QC::kQCFieldName[QC::kTimeEnd].c_str(),
		    QC::kQCFieldName[QC::kMonKey].c_str(),
		    QC::kQCFieldName[QC::kTimeBegin].c_str(),
		    QC::kQCFieldName[QC::kTimeEnd].c_str());
  return table_def;
}

bool TimeQCTable::Fill(QC::MonKey_t type, TDatime tstart, TDatime tend, Double_t mean, Double_t sigma, std::string ref) const
{
  if(!GetConnection()) {

    std::cerr << "Connection not found..." << std::endl;

    return false;
  }

  // For TimeQCTable, we need to check if any existing entry has a time range overlap
  std::string query = Form("SELECT %s, %s FROM %s WHERE %s=%d AND (('%s' < %s AND %s < '%s') OR ('%s' < %s AND %s < '%s')) LIMIT 1",
			   QC::kQCFieldName[QC::kTimeBegin].c_str(),
			   QC::kQCFieldName[QC::kTimeEnd].c_str(),
			   GetTableName().c_str(),
			   QC::kQCFieldName[QC::kMonKey].c_str(),
			   type,
			   tstart.AsSQLString(),
			   QC::kQCFieldName[QC::kTimeBegin].c_str(),
			   QC::kQCFieldName[QC::kTimeBegin].c_str(),
			   tend.AsSQLString(),
			   tstart.AsSQLString(),
			   QC::kQCFieldName[QC::kTimeEnd].c_str(),
			   QC::kQCFieldName[QC::kTimeEnd].c_str(),
			   tend.AsSQLString());

  size_t key = GetConnection()->Query(query);

  if(GetConnection()->FetchRow(key)) {
    
    std::cerr << Form("Entry already exists for MonKey: %d for period '%s' => '%s'",
		      type, 
		      GetConnection()->GetRow(key)->GetTimestamp(0).AsSQLString(),
		      GetConnection()->GetRow(key)->GetTimestamp(1).AsSQLString())
	      << std::endl;
    GetConnection()->ClearResult(key);
    return false;    
  }

  query = Form("INSERT INTO %s (%s,%s,%s,%s,%s,%s)",
	       GetTableName().c_str(),
	       QC::kQCFieldName[QC::kMonKey].c_str(),
	       QC::kQCFieldName[QC::kTimeBegin].c_str(),
	       QC::kQCFieldName[QC::kTimeEnd].c_str(),
	       QC::kQCFieldName[QC::kMean].c_str(),
	       QC::kQCFieldName[QC::kSigma].c_str(),
	       QC::kQCFieldName[QC::kReference].c_str());

  query += Form(" Values (%d, '%s', '%s', %g, %g, '%s')",
		type,
		tstart.AsSQLString(),
		tend.AsSQLString(),
		mean,
		sigma,
		ref.c_str());

  return GetConnection()->Exec(query);

}

bool TimeQCTable::Fetch() 
{
  if(_res_key==DB::INVALID_KEY) {
    
    std::cerr << "Fetch(): call this function upon successful Load() call!" << std::endl;
    
    return false;
  }
  
  if(!(GetConnection()->FetchRow(_res_key))) return false;
  
  _tstart = GetConnection()->GetRow(_res_key)->GetTimestamp(1);
  _tend   = GetConnection()->GetRow(_res_key)->GetTimestamp(2);
  _mean   = GetConnection()->GetRow(_res_key)->GetDouble(3);
  _sigma  = GetConnection()->GetRow(_res_key)->GetDouble(4);
  _ref    = GetConnection()->GetRow(_res_key)->GetString(5);
  _tlog   = GetConnection()->GetRow(_res_key)->GetTimestamp(6);
  return true;
}

void TimeQCTable::Initialize() {
  
  QCTableBase::Initialize();
  
}

#endif
