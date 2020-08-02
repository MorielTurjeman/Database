/*
 * Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#pragma comment(lib, "mysqlcppconn8.lib")

#include <iostream>
#include <mysqlx/xdevapi.h>

using ::std::cout;
using ::std::endl;
using namespace ::mysqlx;
int main(int argc, const char* argv[])
{
	
		const char* url = (argc > 1 ? argv[1] : "mysqlx://mysqluser:mysqlpassword@178.79.166.104");
		cout << "Creating session on " << url
			<< " ..." << endl;
		Session sess(url);
		sess.sql("USE bookstore").execute();

		//define query
		auto query = sess.sql(R"(select books.book_id, books.bookName, count(books.book_id)
									from         books_in_shipments
													inner join store_purchase on books_in_shipments.store_purchase_id = store_purchase.store_purchase_id
											inner join books on store_purchase.book_id = books.book_id
									where         books.bookName = ?
									group by
											books.book_id)");

		query.bind("The Great Gatsby"); // set the paramenters
		auto set = query.execute(); //run the query
		set.count();
		auto rows = set.fetchAll();

		for (auto row : rows)
		{
			std::cout << row.get(0) << ", " << row.get(1) << ", " << row.get(2);
		}


		/*auto set = sess.sql("Select * from bookstore.books").execute();
		auto rows = set.fetchAll();
		cout << "Printing rows " << endl;
		for (auto row : rows)
		{
			cout << row.get(1) << "\n";
		}*/
		
		sess.close();
		
}