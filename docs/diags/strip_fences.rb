#!/usr/bin/ruby

@destdir = "mmd/"

def strip_md_fences(source, dest)
    copy = false
    lineno = 0
    puts "Stripping #{source} to #{dest}"
    File.foreach(source) do |line|
        lineno += 1
        if line.start_with?("```")
            if copy
                puts "Found block closure at line #{lineno}: exiting"
                break
            end
            puts "Found code block at line #{lineno}, it should be the diagram"
            copy = (not copy)
            next
        end
        File.write(dest, line, mode: "a") if copy
    end
end

def main
    puts "Batch job started on #{Time.now.ctime}\n\n"

    if Dir.exist?(@destdir)
        Dir.each_child(@destdir) do |child|
            puts "Cleaning up leftover file #{@destdir+child}"
            File.delete(@destdir+child)
        end
    else
        Dir.mkdir(@destdir)
    end

    index = 0
    entrycount = 0

    Dir.entries(".").each { |entry| entrycount += 1 if entry.end_with?(".md") }

    Dir.glob("*.md") do |mdfile|
        index += 1
        dest = @destdir
        puts "\nProcessing entry #{mdfile} (#{index}/#{entrycount})"
        dest = dest + mdfile.delete_suffix(".md") + ".mmd"
        strip_md_fences(mdfile, dest)
    end

    puts "\nBatch job completed on #{Time.now.ctime}"
    return
end

main