import pandas as pd

def filter_rows(df, attribute):
    max_sample_count = df.loc[0, attribute]
    filtered_df = pd.DataFrame(columns=df.columns)
    
    for index, row in df.iterrows():
        sample_6_86 = row[attribute]
        
        if sample_6_86 >= max_sample_count:
            filtered_df = filtered_df._append(row)
            max_sample_count = sample_6_86
            
    return filtered_df

# import data
df_warmup_pH_6_86 = pd.read_excel("warmup_pH_6_86.xls")
df_warmup_pH_4_01 = pd.read_excel("warmup_pH_4_01.xls")
df_cooldown_pH_4_01 = pd.read_excel("cooldown_pH_4_01.xls")

# slice data into subsets
df_warmup_pH_6_86 = df_warmup_pH_6_86[["CLOCK","SAMPLE","pH_mV","T1"]]
df_warmup_pH_4_01 = df_warmup_pH_4_01[["CLOCK","SAMPLE","pH","T1"]]
df_cooldown_pH_4_01 = df_cooldown_pH_4_01[["CLOCK","SAMPLE","pH","T1"]]

# rename columns
df_warmup_pH_6_86.rename(columns={'pH_mV': 'mV'}, inplace=True)
df_warmup_pH_4_01.rename(columns={'pH': 'mV'}, inplace=True)
df_cooldown_pH_4_01.rename(columns={'pH': 'mV'}, inplace=True)
df_warmup_pH_6_86.rename(columns={'T1': '°C'}, inplace=True)
df_warmup_pH_4_01.rename(columns={'T1': '°C'}, inplace=True)
df_cooldown_pH_4_01.rename(columns={'T1': '°C'}, inplace=True)

# drop rows with any null values
df_warmup_pH_6_86.dropna(inplace=True)
df_warmup_pH_4_01.dropna(inplace=True)
df_cooldown_pH_4_01.dropna(inplace=True)

# set "SAMPLE" type as int
df_warmup_pH_6_86['SAMPLE'] = df_warmup_pH_6_86['SAMPLE'].astype(int)

# inner join on '°C'
df = pd.merge(df_warmup_pH_4_01, df_warmup_pH_6_86, on='°C', how='inner')

# Moving °C column to the first position
df = df[['°C'] + [col for col in df.columns if col != '°C']]

# rename columns
df.rename(columns={'CLOCK_x': 'CLOCK_4_01'}, inplace=True)
df.rename(columns={'SAMPLE_x': 'SAMPLE_4_01'}, inplace=True)
df.rename(columns={'mV_x': 'mV_4_01'}, inplace=True)
df.rename(columns={'CLOCK_y': 'CLOCK_6_86'}, inplace=True)
df.rename(columns={'SAMPLE_y': 'SAMPLE_6_86'}, inplace=True)
df.rename(columns={'mV_y': 'mV_6_86'}, inplace=True)

# Sorting by °C
df.sort_values(by='°C', inplace=True)

# Filter rows based on condition
df = filter_rows(df, "SAMPLE_4_01")
df = filter_rows(df, "SAMPLE_6_86")

# Save DataFrame as Excel file
df.to_excel("warmup_pH_4_01_6_86.xlsx", index=False)

print("DataFrame saved as warmup_pH_4_01_6_86.xlsx")

