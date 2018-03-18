test()
{
File writename = new File(outputPath);
            writename.createNewFile();
codeLine*/
            BufferedWriter out = new BufferedWriter(new FileWriter(writename));
			
    //noteLine
            out.write(outputBuffer);
			
/*noteLine
/*noteLine
*/
/*noteLine*/
/*noteLine
//noteLine
*/codeLine
            out.flush();
            out.close();
}//noteLine
for(){
}/*noteLine*/